#include "Epoller.h"
#include <cassert>
#include <iostream>

using namespace std;

const int EVENTSNUM = 4096;

Epoller::Epoller() : epollfd_(epoll_create1(EPOLL_CLOEXEC)), events_(EVENTSNUM) {
  assert(epollfd_ != -1);
}


void Epoller::epoll_add(const int &fd, __uint32_t fd_events_, int timeout) {
  struct epoll_event event {};
  event.data.fd = fd;
  event.events = fd_events_;
  if (epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &event) < 0) {
    std::cout << "epoll add error!" << std::endl;
  } else {
    std::cout << "epoll add success!" << std::endl;
  }
}
void Epoller::epoll_mod(const int &fd, __uint32_t fd_events_, int timeout) {
  struct epoll_event event {};
  event.data.fd = fd;
  event.events = fd_events_;
  if (epoll_ctl(epollfd_, EPOLL_CTL_MOD, fd, &event) < 0) {
    std::cout << "epoll mod error!";
  }
}
void Epoller::epoll_del(const int &fd) {
  if (epoll_ctl(epollfd_, EPOLL_CTL_DEL, fd, nullptr) < 0) {
    cout << "epoll del error!";
  }
}
int Epoller::poll() {
  return epoll_wait(epollfd_, &*events_.begin(), EVENTSNUM, 1000);
}

const std::vector<epoll_event> &Epoller::getEvents() const {
  return events_;
}
