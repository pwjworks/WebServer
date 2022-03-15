#include "SimpleEpollServer.h"
#include "util.h"
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <unistd.h>

using namespace std;

SimpleEpollServer::SimpleEpollServer(int port) : Server(port),
                                                 epoller_(make_shared<Epoller>()),
                                                 http_data_(make_shared<HttpData>()) {
}


bool SimpleEpollServer::accept_new_conn() {
  struct sockaddr_in clitenaddr {};
  socklen_t clientaddrlen = sizeof clitenaddr;
  // 接收新连接
  int clientfd = accept(listenfd_, (struct sockaddr *) &clitenaddr, &clientaddrlen);
  if (clientfd == -1) return false;
  if (!set_nonblock(clientfd)) return false;
  // 加入到epoller中，监听此描述符
  epoller_->epoll_add(clientfd, EPOLLIN, 1000);
  return true;
}

void SimpleEpollServer::handle_read(int fd) {
  // 简单打印客户端的信息
  char *buf = new char[1024];
  int m = recv(fd, buf, 1024, 0);
  if (m < 0) return;
  if (m == 0)
    epoller_->epoll_del(fd);
  else {
    cout << "recv from client: " << fd << "," << buf << endl;
    //    http_data_->setInBuffer(string(buf));
    //    http_data_->parse();
    string bufs = "HTTP/1.1 200 OK\r\nServer: myhttp\r\nConnection: close\r\nContent-type: text/html\r\n\r\n<h1>Hello World!</h1>\n";
    ssize_t n = writen(fd, bufs);
    cout << n << endl;
    if (n > 0) {
      epoller_->epoll_del(fd);
      close(fd);
    }
  }
}


void SimpleEpollServer::start() {
  // 把监听文件描述符加入epoller中
  epoller_->epoll_add(listenfd_, EPOLLIN, 1000);

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

void SimpleEpollServer::handle_write(int fd) {
  //cout << "********************" << endl;
  //cout << http_data_->getOutBuffer() << endl;
  string buf = "HTTP/1.1 200 OK\r\nConnection: Close\r\nContent-Length: 11\r\nContent-type: text/plain\r\n\r\n";
  string buf1 = "Hello World";
}
