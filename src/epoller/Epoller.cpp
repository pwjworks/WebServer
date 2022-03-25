#include "Epoller.h"
#include <cassert>
#include <iostream>

using namespace std;

const int EVENTSNUM = 4096;
const int EPOLLWAIT_TIME = 10000;

typedef std::shared_ptr<Channel> ChannelPtr;

Epoller::Epoller() : epollfd_(epoll_create1(EPOLL_CLOEXEC)), events_(EVENTSNUM) {
  assert(epollfd_ > 0);
}


void Epoller::epoll_add(ChannelPtr request, int timeout) {
  int fd = request->get_fd();
  struct epoll_event event {};
  event.data.fd = fd;
  event.events = request->get_events();

  fd2chan_[fd] = request;
  if (epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &event) < 0) {
    std::cout << "epoll add error!" << std::endl;
    fd2chan_[fd].reset();
  }
}
void Epoller::epoll_mod(ChannelPtr request, int timeout) {
  int fd = request->get_fd();
  struct epoll_event event {};
  event.data.fd = fd;
  event.events = request->get_events();
  if (epoll_ctl(epollfd_, EPOLL_CTL_MOD, fd, &event) < 0) {
    std::cout << "epoll mod error!";
    fd2chan_[fd].reset();
  }
}
void Epoller::epoll_del(ChannelPtr request) {
  int fd = request->get_fd();
  if (epoll_ctl(epollfd_, EPOLL_CTL_DEL, fd, nullptr) < 0) {
    cout << "epoll del error!";
  }
  fd2chan_[fd].reset();
  fd2http_[fd].reset();
}
std::vector<ChannelPtr> Epoller::poll() {
  while (true) {
    int event_count = epoll_wait(epollfd_, &*events_.begin(), EVENTSNUM, EPOLLWAIT_TIME);
    if (event_count < 0) perror("epoll wait error");
    std::vector<ChannelPtr> req_data = get_events_request(event_count);
    if (!req_data.empty()) return req_data;
  }
}

const std::vector<epoll_event> &Epoller::getEvents() const {
  return events_;
}
vector<ChannelPtr> Epoller::get_events_request(int events_num) {
  std::vector<ChannelPtr> req_data;
  for (int i = 0; i < events_num; ++i) {
    // 获取有事件产生的描述符
    int fd = events_[i].data.fd;

    ChannelPtr cur_req = fd2chan_[fd];

    if (cur_req) {
      cur_req->set_revents(events_[i].events);
      cur_req->set_events(0);
      req_data.push_back(cur_req);
    }
  }
  return req_data;
}
