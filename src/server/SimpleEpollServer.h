#include "EventLoopThreadPool.h"
#include "Server.h"
#include <memory>
#pragma once


class SimpleEpollServer : public Server {
public:
  using EventLoopThreadPoolPtr = std::shared_ptr<EventLoopThreadPool>;
  explicit SimpleEpollServer(int port);
  ~SimpleEpollServer() override = default;
  void start() override;


private:
  bool quit_;
  EventLoopThreadPoolPtr threadpool_ptr_;
};
