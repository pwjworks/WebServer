#include "Server.h"
#include <HttpData.h>
#include <cassert>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <unistd.h>

using namespace std;


Server::Server(int port) : listenfd_(socket(AF_INET, SOCK_STREAM, 0)) {
  assert(listenfd_ != -1);
  assert(server_fd_init(port) == true);
}

Server::~Server() {
  if (listenfd_ != -1) close(listenfd_);
}

bool Server::server_fd_init(int port) {
  // 检查port值，取正确区间范围
  if (port < 0 || port > 65535) return -1;

  struct sockaddr_in bindaddr {};
  bzero((char *) &bindaddr, sizeof(bindaddr));
  bindaddr.sin_family = AF_INET;
  bindaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  bindaddr.sin_port = htons(port);

  // 绑定端口
  if (bind(listenfd_, (struct sockaddr *) &bindaddr, sizeof(bindaddr)) == -1) {
    cout << "bind listen socket error." << endl;
    return false;
  }

  // 监听端口
  if (listen(listenfd_, SOMAXCONN) == -1) {
    cout << "listen error." << endl;
    return false;
  }

  return true;
}

int Server::accept_new_conn() {
  struct sockaddr_in clitenaddr {};
  socklen_t clientaddrlen = sizeof clitenaddr;
  // 接收新连接
  int clientfd = accept(listenfd_, (struct sockaddr *) &clitenaddr, &clientaddrlen);
  if (clientfd < 0) return -1;

  return clientfd;
}
