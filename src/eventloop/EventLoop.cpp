//
// Created by pwjworks on 2022/2/11.
//

#include "EventLoop.h"
#include <iostream>
#include <sys/epoll.h>
#include <sys/eventfd.h>

using namespace std;

void EventLoop::runInLoop(Functor &&cb) {
  if (isInLoopThread())
    cb();
  else
    queueInLoop(std::move(cb));
}

void EventLoop::queueInLoop(std::function<void()> &&cb) {
  {
    lock_guard<mutex> lck(mutex_);
    pendingFunctors_.emplace_back(std::move(cb));
  }
  if (!isInLoopThread() || callingPendingFunctors_) wakeup();
}

void EventLoop::wakeup() {
  uint64_t one = 1;
  ssize_t n =
}