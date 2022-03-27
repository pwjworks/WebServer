#include "Epoller.h"
#include "Util.h"
#include <cassert>
#include <iostream>
#include <netinet/in.h>


const int EVENTSNUM = 4096;

Epoller::Epoller() : listenfd_(-1), wakeupfd_(-1), epollfd_(epoll_create1(EPOLL_CLOEXEC)), events_(EVENTSNUM) {
  assert(epollfd_ != -1);
}

Epoller::Epoller(int listenfd, int wakeupfd) : listenfd_(listenfd), wakeupfd_(wakeupfd),
                                               epollfd_(epoll_create1(EPOLL_CLOEXEC)), events_(EVENTSNUM) {
  assert(epollfd_ != -1);
}


void Epoller::epoll_add(const int &fd, __uint32_t fd_events_, int timeout) {
  struct epoll_event event {};
  event.data.fd = fd;
  event.events = fd_events_;
  fd2http_[fd] = std::make_shared<HttpData>(fd);
  if (epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &event) < 0) {
    std::cout << "epoll add error!" << std::endl;
    fd2http_[fd]->reset();
  }
}
void Epoller::epoll_mod(const int &fd, __uint32_t fd_events_, int timeout) {
  struct epoll_event event {};
  event.data.fd = fd;
  event.events = fd_events_;
  if (epoll_ctl(epollfd_, EPOLL_CTL_MOD, fd, &event) < 0) {
    fd2http_[fd]->reset();
    std::cout << "epoll mod error!";
  }
}
void Epoller::epoll_del(const int &fd) {
  if (epoll_ctl(epollfd_, EPOLL_CTL_DEL, fd, nullptr) < 0) {
    fd2http_[fd]->reset();
    std::cout << "epoll del error!";
  }
}
void Epoller::poll() {
  int event_count = epoll_wait(epollfd_, &*events_.begin(), EVENTSNUM, 1000);
  if (event_count < 0) perror("epoll wait error");
  for (int i = 0; i < event_count; ++i) {
    if (events_[i].data.fd == wakeupfd_)
      continue;
    handle_event(events_[i].data.fd, events_[i].events);
  }
}

void Epoller::handle_event(int fd, __uint32_t revents_) {
  if ((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN)) {
    return;
  }
  if (revents_ & EPOLLERR) {
  }
  int clientfd;
  if (fd == listenfd_ && (revents_ & EPOLLIN) && (clientfd = handle_new_conn()) != -1) {
    if (set_nonblock(clientfd))
      epoll_add(clientfd, EPOLLIN | EPOLLET, 1000);
    return;
  }
  if (revents_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)) {
    fd2http_[fd]->handle_read();
    fd2http_[fd]->parse();
    fd2http_[fd]->handle_write();
    fd2http_[fd]->handle_conn();
    fd2http_[fd]->reset();
  }
}

const std::vector<epoll_event> &Epoller::getEvents() const {
  return events_;
}

int Epoller::handle_new_conn() {
  struct sockaddr_in clitenaddr {};
  socklen_t clientaddrlen = sizeof clitenaddr;
  // 接收新连接
  int clientfd = accept(listenfd_, (struct sockaddr *) &clitenaddr, &clientaddrlen);
  if (clientfd < 0) return -1;

  return clientfd;
}
