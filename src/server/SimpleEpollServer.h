#include "Channel.h"
#include "Epoller.h"
#include "Server.h"
#include <memory>
#pragma once


class SimpleEpollServer : public Server {
public:
  typedef std::shared_ptr<Epoller> EpollerPtr;
  typedef std::shared_ptr<Channel> ChannelPtr;

  explicit SimpleEpollServer(int port);
  ~SimpleEpollServer() override = default;
  void handle_events(int fd) override;
  void start() override;

  void handle_new_conn();

private:
  ChannelPtr accept_channel_;
  EpollerPtr epoller_;
};
