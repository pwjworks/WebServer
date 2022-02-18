//
// Created by pwjworks on 2022/2/11.
//

#include "Eventloop.h"
#include <iostream>
#include <sys/epoll.h>
#include <sys/eventfd.h>

void Eventloop::runInLoop(Functor &&cb) const {
  if (isInLoopThread())
    cb();
  else
    queueInLoop(std::move(cb));
}

void Eventloop::queueInLoop(std::function<void()> &&cb) {
  {
    MutexLockGuard lock(mutex_);
    pendingFunctors_.emplace_back(std::move(cb));
  }
  if (!isInLoopThread() || callingPendingFunctors_) wakeup();
}
