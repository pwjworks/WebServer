//
// Created by pwjworks on 2022/2/9.
//
#include "echo_server.h"
#include <cassert>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <unistd.h>

echo_server::echo_server(int port) : Server(port) {
}

void echo_server::start() {
  while ((event_count_ = getEpoller()->poll()) <= 0) std::cout << event_count_ << std::endl;
  assert(event_count_ >= 0);
  std::cout << "poll finish!" << std::endl;
  for (size_t i = 0; i < event_count_; i++) {
    if (getEpoller()->getEvents()[i].events & EPOLLIN) {
      handleNewConn();
    } else {
    }
  }
  std::cout << "start out!" << std::endl;
}


void echo_server::handleNewConn() {
  struct sockaddr_in client_addr {};
  memset(&client_addr, 0, sizeof(struct sockaddr_in));
  socklen_t client_addr_len = sizeof(client_addr);
  int accept_fd = 0;
  // 接受连接
  if ((accept_fd = accept(getListenFd(), (struct sockaddr *) &client_addr,
                          &client_addr_len)) > 0) {
    // 设置非阻塞
    setSocketNonBlocking(accept_fd);
    std::cout << "accept new conn!" << std::endl;
    getEpoller()->epoll_add(accept_fd, EPOLLIN, 1000);
  }
}

int echo_server::setSocketNonBlocking(int fd) {
  int flag = fcntl(fd, F_GETFD, 0);
  if (flag == -1) return -1;
  flag |= O_NONBLOCK;
  if (fcntl(fd, F_SETFL, flag) == -1) return -1;
  return 0;
}

void echo_server::handleRead(int fd) {
  int m;
}

void echo_server::handleWrite() {
}
