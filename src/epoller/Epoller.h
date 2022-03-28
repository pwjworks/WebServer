#include "HttpData.h"
#include <memory>
#include <sys/epoll.h>
#include <vector>

#pragma once

class Epoller {
public:
  Epoller(int wakeupfd);
  ~Epoller() = default;
  using HttpDataPtr = std::shared_ptr<HttpData>;
  void epoll_add(const int &fd, __uint32_t fd_events_, int timeout);
  void epoll_mod(const int &fd, __uint32_t fd_events_, int timeout);
  void epoll_del(const int &fd);
  void handle_event(int fd, __uint32_t revents_);
  void handle_wakeup();

  void poll();

public:
  const std::vector<epoll_event> &getEvents() const;

private:
  static const int MAXFDS = 10000;
  int wakeupfd_;
  int epollfd_;
  std::vector<epoll_event> events_;
  std::shared_ptr<HttpData> fd2http_[MAXFDS];
};
