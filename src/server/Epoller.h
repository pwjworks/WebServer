//
// Created by pwjworks on 2022/2/8.
//
#pragma once
#include "Timer.h"
#include <Channel.h>
#include <sys/epoll.h>
#include <vector>

class Epoller {
public:
  Epoller();
  ~Epoller();
  void epoll_add(const SP_Channel &request, int timeout);
  void epoll_mod(const SP_Channel &request, int timeout);
  void epoll_del(const SP_Channel &request);
  std::vector<std::shared_ptr<Channel>> poll();
  std::vector<std::shared_ptr<Channel>> getEventsRequest(int events_num);
  void add_timer(std::shared_ptr<Channel> request_data, int timeout);
  int getEpollFd() const { return epollFd_; }
  void handleExpired();

private:
  static const int MAXFDS = 100000;
  int epollFd_;
  std::vector<epoll_event> events_;
  std::shared_ptr<Channel> fd2chan_[MAXFDS];
  std::shared_ptr<HttpData> fd2http_[MAXFDS];
  TimerManager timerManager_;

public:
  const std::vector<epoll_event> &getEvents() const;
};
