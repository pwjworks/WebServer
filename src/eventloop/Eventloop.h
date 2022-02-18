//
// Created by pwjworks on 2022/2/11.
//
#pragma once
#include "Epoller.h"
#include <functional>
#include <memory>
#include <thread>
#include <vector>

class Eventloop {
public:
  typedef std::function<void()> Functor;
  Eventloop();
  ~Eventloop();
  void loop();
  void quit();
  void runInLoop(Functor &&cb) const;
  bool isInLoopThread() const { return threadId_ == std::CurrentThread::tid(); }
  void queueInLoop(std::function<void()> &&cb);

private:
  bool looping_;
  std::shared_ptr<Epoller> epoller_;

private:
  bool callingPendingFunctors_;
  mutable MutexLock mutex_;
  const pid_t threadId_;
  std::vector<Functor> pendingFunctors_;
};
