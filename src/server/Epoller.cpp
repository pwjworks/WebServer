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

void Epoller::epoll_add(const SP_Channel &request, int timeout) {
  int fd = request->getFd();
  if (timeout > 0) {
    add_timer(request, timeout);
    fd2http_[fd] = request->getHolder();
  }
  struct epoll_event event {};
  event.data.fd = fd;
  event.events = request->getEvents();

  request->EqualAndUpdateLastEvents();

  fd2chan_[fd] = request;
  if (epoll_ctl(epollFd_, EPOLL_CTL_ADD, fd, &event) < 0) {
    perror("epoll_add error");
    fd2chan_[fd].reset();
  }
}

void Epoller::epoll_mod(const SP_Channel &request, int timeout) {
  if (timeout > 0) add_timer(request, timeout);
  int fd = request->getFd();
  if (!request->EqualAndUpdateLastEvents()) {
    struct epoll_event event {};
    event.data.fd = fd;
    event.events = request->getEvents();
    if (epoll_ctl(epollFd_, EPOLL_CTL_MOD, fd, &event) < 0) {
      perror("epoll_mod error");
      fd2chan_[fd].reset();
    }
  }
}

void Epoller::epoll_del(const SP_Channel &request) {
  int fd = request->getFd();
  struct epoll_event event;
  event.data.fd = fd;
  event.events = request->getLastEvents();
  if (epoll_ctl(epollFd_, EPOLL_CTL_DEL, fd, &event) < 0) {
    perror("epoll_del error");
  }
  fd2chan_[fd].reset();
  fd2http_[fd].reset();
}

std::vector<std::shared_ptr<Channel>> Epoller::poll() {
  while (true) {
    int event_count = epoll_wait(epollFd_, &*events_.begin(), events_.size(), EPOLLWAIT_TIME);
    if (event_count < 0) perror("epoll wait error");
    std::vector<SP_Channel> req_data = getEventsRequest(event_count);
    if (event_count == 0) continue;
    if (!req_data.empty()) return req_data;
  }
}
const std::vector<epoll_event> &Epoller::getEvents() const {
  return events_;
}

void Epoller::add_timer(SP_Channel request_data, int timeout) {
  std::shared_ptr<HttpData> t = request_data->getHolder();
  if (t)
    timerManager_.addTimer(t, timeout);
  // TODO logger
}

void Epoller::handleExpired() {
  timerManager_.handleExpiredEvent();
}

std::vector<std::shared_ptr<Channel>> Epoller::getEventsRequest(int events_num) {
  std::vector<SP_Channel> req_data;
  for (int i = 0; i < events_num; i++) {
    int fd = events_[i].data.fd;
    SP_Channel cur_req = fd2chan_[fd];
    if (cur_req) {
      cur_req->setRevents(events_[i].events);
      cur_req->setEvents(0);
      req_data.emplace_back(cur_req);
    }
    // TODO logger
    return req_data;
  }
}
