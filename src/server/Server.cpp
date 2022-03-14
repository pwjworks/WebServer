#include "Server.h"

#include <cassert>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <unistd.h>

using namespace std;


Server::Server(int port) : listenfd_(socket(AF_INET, SOCK_STREAM, 0)),
                           epoller_(make_shared<Epoller>()),
                           http_data_(make_shared<HttpData>()) {
  assert(listenfd_ != -1);
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

  // 把监听文件描述符加入epoller中
  epoller_->epoll_add(listenfd_, EPOLLIN, 1000);

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

bool Server::accept_new_conn() {
  struct sockaddr_in clitenaddr;
  socklen_t clientaddrlen = sizeof clitenaddr;
  // 接收新连接
  int clientfd = accept(listenfd_, (struct sockaddr *) &clitenaddr, &clientaddrlen);
  if (clientfd == -1) return false;
  if (!set_nonblock(clientfd)) return false;
  // 加入到epoller中，监听此描述符
  epoller_->epoll_add(clientfd, EPOLLIN, 1000);
  return true;
}

void Server::handle_read(int fd) {
  // 简单打印客户端的信息
  char *buf = new char[1024];
  int m = recv(fd, buf, 1024, 0);
  if (m < 0) return;
  if (m == 0)
    epoller_->epoll_del(fd);
  else {
    cout << "recv from client: " << fd << "," << buf << endl;
    http_data_->setInBuffer(string(buf));
    http_data_->parse();
    string buf = "HTTP/1.1 200 OK\r\nContent-type: text/plain\r\n\r\nHello World";
    int n = send(fd, buf.c_str(), static_cast<int>(buf.size()), 0);
    cout << n << endl;
  }
}

void Server::handle_write(int fd) {
  //cout << "********************" << endl;
  //cout << http_data_->getOutBuffer() << endl;
  string buf = "HTTP/1.1 200 OK\r\nConnection: Close\r\nContent-Length: 11\r\nContent-type: text/plain\r\n\r\n";
  string buf1 = "Hello World";
  int n = send(fd, buf.c_str(), static_cast<int>(buf.size()), 0);
  send(fd, buf1.c_str(), static_cast<int>(buf1.size()), 0);
  cout << n << endl;
}


void Server::start() {
  cout << "Running..." << endl;

  while (true) {
    cout << "Running..." << endl;
    int n = epoller_->poll();
    if (n < 0) {
      if (errno == EINTR)
        continue;
      break;
    } else if (n == 0) {
      continue;
    }
    for (size_t i = 0; i < n; ++i) {
      if (epoller_->getEvents()[i].events & EPOLLIN) {
        int curr_fd = epoller_->getEvents()[i].data.fd;
        if (curr_fd == listenfd_ && !accept_new_conn()) cout << "accept error!";
        else {
          handle_read(curr_fd);
          //handle_write(curr_fd);
        }
      }
    }
  }
}
