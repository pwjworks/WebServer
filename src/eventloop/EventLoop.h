//
// Created by pwjworks on 2022/2/11.
//
#pragma once
#include "Epoller.h"
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

class EventLoop {
public:
  typedef std::function<void()> Functor;
  EventLoop();
  ~EventLoop();
  void loop();
  void quit();
  void runInLoop(Functor &&cb) const;
  bool isInLoopThread() const { return threadId_ == std::CurrentThread::tid(); }
  void queueInLoop(std::function<void()> &&cb);

  void wakeup();

private:
  bool looping_;
  std::shared_ptr<Epoller> epoller_;

private:
  bool callingPendingFunctors_;
  mutable std::mutex mutex_;
  const pid_t threadId_;
  std::vector<Functor> pendingFunctors_;
};
