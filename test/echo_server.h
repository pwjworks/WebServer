//
// Created by pwjworks on 2022/2/9.
//
#include "Server.h"
#pragma once

class echo_server : public Server {
public:
  explicit echo_server(int);
  void start();
  /**
  * @brief 处理并接受新的连接
  */
  void handleNewConn();
  void handleRead();
  void handleWrite();

private:
  int event_count_{};
  int setSocketNonBlocking(int fd);
};
