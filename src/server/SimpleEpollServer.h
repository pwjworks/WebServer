#include "EventLoop.h"
#include "Server.h"
#include <memory>
#pragma once


class SimpleEpollServer : public Server {
public:
  using EventLoopPtr = std::shared_ptr<EventLoop>;
  explicit SimpleEpollServer(int port);
  ~SimpleEpollServer() override = default;
  void start() override;


private:
  EventLoopPtr eventloop_ptr_;
};
