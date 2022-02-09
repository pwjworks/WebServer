//
// Created by pwjworks on 2022/2/9.
//

#pragma once
#include "Epoller.h"
#include <memory>

class Server {
public:
  explicit Server(int);
  ~Server();

private:
  int server_fd_init(int);
  int listen_fd_;
  std::shared_ptr<Epoller> epoller_;

public:
  const std::shared_ptr<Epoller> &getEpoller() const;
  int getListenFd() const;
  static const int MAXFDS = 100000;
};
