#include "Epoller.h"
#include "HttpData.h"
#include "Server.h"
#include <memory>
#pragma once


class SimpleEpollServer : public Server {
public:
  typedef std::shared_ptr<Epoller> EpollerPtr;
  typedef std::shared_ptr<HttpData> HttpDataPtr;
  typedef std::shared_ptr<Channel> ChannelPtr;

  explicit SimpleEpollServer(int port);
  ~SimpleEpollServer() override = default;

  void handle_read(int fd) override;
  void start() override;


private:
  __uint32_t revents;
  EpollerPtr epoller_;
  HttpDataPtr http_data_;
  ChannelPtr channel_ptr_;
};
