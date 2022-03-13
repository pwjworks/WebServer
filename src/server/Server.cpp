#include "Server.h"
#include <cassert>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <unistd.h>

using namespace std;

const int EVENTSNUM = 4096;

Server::Server(int port) : listenfd_(socket(AF_INET, SOCK_STREAM, 0)), epollfd_(epoll_create(1)), events_(EVENTSNUM) {
  assert(listenfd_ != -1);
  assert(epollfd_ != -1);
  assert(server_fd_init(port) == true);
  assert(set_nonblock(listenfd_) == true);
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
  epoll_add(listenfd_, EPOLLIN, 1000);

  if (bind(listenfd_, (struct sockaddr *) &bindaddr, sizeof(bindaddr)) == -1) {
    cout << "bind listen socket error." << endl;
    close(listenfd_);
    return false;
  }

  if (listen(listenfd_, SOMAXCONN) == -1) {
    cout << "listen error." << endl;
    close(listenfd_);
    return false;
  }
  return true;
}

bool Server::accept_newconn() {
  struct sockaddr_in clitenaddr;
  socklen_t clientaddrlen = sizeof clitenaddr;
  // 接收新连接
  int clientfd = accept(listenfd_, (struct sockaddr *) &clitenaddr, &clientaddrlen);
  if (clientfd == -1) return false;
  if (!set_nonblock(clientfd)) return false;
  // 加入到epoll中，监听此fd
  epoll_add(clientfd, EPOLLIN, 1000);
  return true;
}

void Server::handleRead(int index) {
  char *buf = new char[1024];
  int fd = events_[index].data.fd;
  int m = recv(fd, buf, 1024, 0);
  if (m <= 0) epoll_del(fd);
  else {
    cout << "recv from client: " << fd << "," << buf << endl;
  }
}


void Server::Start() {
  cout << "Running..." << endl;
  while (true) {
    cout << "Running..." << endl;
    int n = epoll_wait(epollfd_, &*events_.begin(), EVENTSNUM, 1000);
    if (n < 0) {
      if (errno == EINTR)
        continue;
      break;
    } else if (n == 0) {
      continue;
    }
    for (size_t i = 0; i < n; ++i) {
      if (events_[i].events & EPOLLIN) {
        if (events_[i].data.fd == listenfd_ && !accept_newconn()) cout << "accept error!";
        else
          handleRead(i);
      }
    }
  }
}

void Server::epoll_add(const int &fd, __uint32_t fd_events_, int timeout) {
  struct epoll_event event {};
  event.data.fd = fd;
  event.events = fd_events_;
  if (epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &event) < 0) {
    std::cout << "epoll add error!" << std::endl;
  } else {
    std::cout << "epoll add success!" << std::endl;
  }
}

void Server::epoll_mod(const int &fd, __uint32_t fd_events_, int timeout) {
  struct epoll_event event {};
  event.data.fd = fd;
  event.events = fd_events_;
  if (epoll_ctl(epollfd_, EPOLL_CTL_MOD, fd, &event) < 0) {
    std::cout << "epoll mod error!";
  }
}

void Server::epoll_del(const int &fd) {
  if (epoll_ctl(epollfd_, EPOLL_CTL_DEL, fd, nullptr) < 0) {
    std::cout << "epoll del error!";
  }
}
