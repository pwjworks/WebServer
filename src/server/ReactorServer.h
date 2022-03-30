#include "EventLoopThreadPool.h"
#include "Server.h"
#include <memory>
#pragma once


class ReactorServer : public Server {
public:
  using EventLoopThreadPoolPtr = std::shared_ptr<EventLoopThreadPool>;
  explicit ReactorServer(int port);
  ~ReactorServer() override = default;
  void start() override;


private:
  bool quit_;
  EventLoopThreadPoolPtr threadpool_ptr_;
};
