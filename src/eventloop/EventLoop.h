#include "Epoller.h"
#include "HttpData.h"
#include <functional>
#include <memory>

#pragma once

class EventLoop {
public:
  using Functor = std::function<void()>;
  using EpollerPtr = std::shared_ptr<Epoller>;

  EventLoop();
  ~EventLoop();
  void removeFromPoller(int fd) {
    wakeup();
    epoller_ptr->epoll_del(fd);
  }

  void updatePoller(int fd, __uint32_t revents_) {
    wakeup();
    epoller_ptr->epoll_mod(fd, revents_, 1000);
  }

  void addToPoller(int fd, __uint32_t revents_, int timeout = 0) {
    wakeup();
    epoller_ptr->epoll_add(fd, revents_, timeout);
  }

  void wakeup();
  void loop();
  void quit();
  //  void runInLoop(Functor &&cb);
  //  void queueInLoop(Functor &&cb);

private:
  bool looping_;
  bool quit_;
  int wakeup_fd_;
  bool event_handling_;
  EpollerPtr epoller_ptr;
};
