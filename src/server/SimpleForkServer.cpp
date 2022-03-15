#include "SimpleForkServer.h"
#include "util.h"
#include <iostream>
#include <netinet/in.h>
#include <sys/wait.h>
#include <unistd.h>

using namespace std;

SimpleForkServer::SimpleForkServer(int port) : Server(port) {
}

void SimpleForkServer::handle_read(int fd) {
  // 简单打印客户端的信息
  char *buf = new char[1024];
  int n = recv(fd, buf, 1024, 0);
  cout << "recv from client: " << fd << "," << buf << endl;
  //    http_data_->setInBuffer(string(buf));
  //    http_data_->parse();
  string bufs = "HTTP/1.1 200 OK\r\nServer: myhttp\r\nConnection: close\r\nContent-type: text/html\r\n\r\n<h1>Hello World!</h1>\n";
  ssize_t n1 = writen(fd, bufs);
  cout << n1 << endl;
}


void SimpleForkServer::start() {
  while (true) {
    cout << "running..." << endl;
    int clint_fd = accept_new_conn();
    if (clint_fd == -1) continue;
    pid_t pid = fork();
    if (pid == -1) {
      close(clint_fd);
      continue;
    }
    if (pid == 0) {
      handle_read(clint_fd);
      close(clint_fd);
      // 处理完毕后立即退出进程
      break;
    } else {
      int stat;
      waitpid(-1, &stat, WNOHANG);
      close(clint_fd);
    }
    cout << "running..." << endl;
  }
}

void SimpleForkServer::handle_write(int fd) {
  string buf = "HTTP/1.1 200 OK\r\nConnection: Close\r\nContent-Length: 11\r\nContent-type: text/plain\r\n\r\n";
  string buf1 = "Hello World";
}
