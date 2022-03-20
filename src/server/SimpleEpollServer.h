#include "Epoller.h"
#include "Server.h"
#include <memory>
#pragma once


class SimpleEpollServer : public Server {
public:
  typedef std::shared_ptr<Epoller> EpollerPtr;

  explicit SimpleEpollServer(int port);
  ~SimpleEpollServer() override = default;
  void handle_events(int fd) override;
  void start() override;


private:
  EpollerPtr epoller_;
};
