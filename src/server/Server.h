//
// Created by pwjworks on 2022/2/9.
//

#pragma once


class Server {
public:
  explicit Server(int);
  int server_fd_init(int);


private:
  int listen_fd_;
};
