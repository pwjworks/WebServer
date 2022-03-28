#include "EventLoop.h"
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#pragma once

class EventLoopThreadPool {
public:
  EventLoopThreadPool() = default;
  ~EventLoopThreadPool() = default;

  void init(int num);

  void start();

  void add_fd(int fd);

private:
  void Run(int index);
  int thread_num_ = 0;// 线程数量
  int next_;
  std::mutex mux_;
  std::vector<std::shared_ptr<std::thread>> threads_;
  std::vector<std::shared_ptr<EventLoop>> eventloop_;
};
