//
// Created by pwjworks on 2022/2/8.
//

#include "Epoller.h"
#include <cassert>
#include <iostream>

const int EVENTSNUM = 4096;
const int EPOLLWAIT_TIME = 10000;

Epoller::Epoller() : epollFd_(epoll_create1(EPOLL_CLOEXEC)), events_(EVENTSNUM) {
  assert(epollFd_ > 0);
}
Epoller::~Epoller() = default;

void Epoller::epoll_add(int fd, __uint32_t fd_events_, int timeout) const {
  struct epoll_event event {};
  event.data.fd = fd;
  event.events = fd_events_;
  if (epoll_ctl(epollFd_, EPOLL_CTL_ADD, fd, &event) < 0) {
    std::cout << "epoll add error!" << std::endl;
  } else {
    std::cout << "epoll add success!" << std::endl;
  }
}

void Epoller::epoll_mod(int fd, __uint32_t fd_events_, int timeout) const {
  struct epoll_event event {};
  event.data.fd = fd;
  event.events = fd_events_;
  if (epoll_ctl(epollFd_, EPOLL_CTL_MOD, fd, &event) < 0) {
    std::cout << "epoll mod error!";
  }
}

void Epoller::epoll_del(int fd, __uint32_t fd_events_) const {
  struct epoll_event event {};
  event.data.fd = fd;
  event.events = fd_events_;
  if (epoll_ctl(epollFd_, EPOLL_CTL_DEL, fd, &event) < 0) {
    std::cout << "epoll del error!";
  }
}

int Epoller::poll() {
  while (true) {
    int event_count = epoll_wait(epollFd_, &*events_.begin(), events_.size(), EPOLLWAIT_TIME);
    if (event_count < 0) std::cout << "epoll wait error";
    if (event_count == 0) continue;
    return event_count;
  }
}
const std::vector<epoll_event> &Epoller::getEvents() const {
  return events_;
}
