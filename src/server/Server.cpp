#include "Server.h"
#include "util.h"
#include <cassert>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <unistd.h>

using namespace std;


Server::Server(int port) : listenfd_(socket(AF_INET, SOCK_STREAM, 0)) {
  assert(listenfd_ != -1);
  assert(set_nonblock(listenfd_) == true);
  assert(server_fd_init(port) == true);
}

Server::~Server() {
  if (listenfd_ != -1) close(listenfd_);
}

bool Server::set_nonblock(int fd) {
  int oldSocketFlag = fcntl(fd, F_GETFL, 0);
  int newSocketFlag = oldSocketFlag | O_NONBLOCK;
  if (fcntl(listenfd_, F_SETFL, newSocketFlag) == -1) {
    close(listenfd_);
    cout << "set listenfd to nonblock error" << endl;
    return false;
  }
  return true;
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
    close(listenfd_);
    return false;
  }

  // 监听端口
  if (listen(listenfd_, SOMAXCONN) == -1) {
    cout << "listen error." << endl;
    close(listenfd_);
    return false;
  }

  return true;
}
