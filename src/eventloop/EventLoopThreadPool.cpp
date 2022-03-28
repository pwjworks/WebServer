#include "EventLoopThreadPool.h"
#include <iostream>

void EventLoopThreadPool::init(int num) {
  std::unique_lock<std::mutex> lock(mux_);
  this->thread_num_ = num;
  std::cout << "Thread pool init!" << num << std::endl;
  for (int i = 0; i < thread_num_; i++) {
    auto loop_ = std::make_shared<EventLoop>();
    eventloop_.emplace_back(loop_);
  }
}
void EventLoopThreadPool::start() {
  std::unique_lock<std::mutex> lock(mux_);
  if (thread_num_ <= 0) {
    perror("Please Init thread pool");
    return;
  }
  for (int i = 0; i < thread_num_; i++) {
    auto th = std::make_shared<std::thread>(&EventLoopThreadPool::Run, this, i);
    threads_.emplace_back(th);
  }
}

void EventLoopThreadPool::Run(int index) {
  eventloop_[index]->loop();
}

void EventLoopThreadPool::add_fd(int fd) {
  std::unique_lock<std::mutex> lock(mux_);
  if (!eventloop_.empty()) {
    eventloop_[next_]->addToPoller(fd, EPOLLIN | EPOLLET);
    next_ = (next_ + 1) % thread_num_;
  }
}
