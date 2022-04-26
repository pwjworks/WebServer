#include "Epoller.h"
#include "Util.h"
#include <cassert>
#include <iostream>
#include <unistd.h>


const int EVENTSNUM = 4096;


Epoller::Epoller(int wakeupfd) : wakeupfd_(wakeupfd),
                                 epollfd_(epoll_create1(EPOLL_CLOEXEC)), events_(EVENTSNUM) {
  assert(epollfd_ != -1);
}


void Epoller::epoll_add(const int &fd, __uint32_t fd_events_, int timeout) {
  struct epoll_event event {};
  event.data.fd = fd;
  event.events = fd_events_;
  if (fd2http_[fd] != nullptr) fd2http_[fd].reset();
  fd2http_[fd] = std::make_shared<HttpData>(fd);
  if (epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &event) < 0) {
    std::cout << "epoll add error!" << std::endl;
  }
}

void Epoller::epoll_mod(const int &fd, __uint32_t fd_events_, int timeout) {
  struct epoll_event event {};
  event.data.fd = fd;
  event.events = fd_events_;
  if (epoll_ctl(epollfd_, EPOLL_CTL_MOD, fd, &event) < 0) {
    fd2http_[fd].reset();
    std::cout << "epoll mod error!" << std::endl;
  }
}
void Epoller::epoll_del(const int &fd) {
  if (epoll_ctl(epollfd_, EPOLL_CTL_DEL, fd, nullptr) < 0) {
    std::cout << "epoll del error!" << std::endl;
    //    std::cout << errno << std::endl;
  }
}
void Epoller::poll() {
  int event_count = epoll_wait(epollfd_, &*events_.begin(), EVENTSNUM, 1000);
  if (event_count < 0) perror("epoll wait error");
  for (int i = 0; i < event_count; ++i) {
    if (events_[i].data.fd == wakeupfd_) {
      handle_wakeup();
      continue;
    }
    handle_event(events_[i].data.fd, events_[i].events);
  }
}

void Epoller::handle_event(int fd, __uint32_t revents_) {
  if ((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN)) {
    return;
  }
  if (revents_ & EPOLLERR) {
  }
  if (revents_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)) {
    if (fd2http_[fd] == nullptr)
      return;
    fd2http_[fd]->handle_read();
    fd2http_[fd]->parse();
    fd2http_[fd]->handle_write();
    fd2http_[fd]->handle_conn();
    fd2http_[fd].reset();
  }
}

const std::vector<epoll_event> &Epoller::getEvents() const {
  return events_;
}

void Epoller::handle_wakeup() {
  uint64_t one = 1;
  bool _unused = false;
  readn(wakeupfd_, &one, sizeof one, _unused);
}
