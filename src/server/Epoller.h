//
// Created by pwjworks on 2022/2/8.
//
#pragma once
#include <sys/epoll.h>
#include <vector>

class Epoller {
public:
  Epoller();
  ~Epoller();
  void epoll_add(int fd, __uint32_t events_, int timeout) const;
  void epoll_mod(int fd, __uint32_t fd_events_, int timeout) const;
  void epoll_del(int fd, __uint32_t fd_events_) const;
  int poll();

private:
  static const int MAXFDS = 100000;
  int epollFd_;
  std::vector<epoll_event> events_;

public:
  const std::vector<epoll_event> &getEvents() const;
};
