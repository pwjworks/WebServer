#include "SimpleForkServer.h"
#include "Util.h"
#include <iostream>
#include <netinet/in.h>
#include <sys/wait.h>
#include <unistd.h>

using namespace std;

SimpleForkServer::SimpleForkServer(int port) : Server(port),
                                               http_data_(make_shared<HttpData>()) {
}

void SimpleForkServer::handle_read(int fd) {
  // 简单打印客户端的信息
  char *buf = new char[1024];
  int m = recv(fd, buf, 1024, 0);
  if (m > 0) {
    //cout << "recv from client: " << fd << "," << buf << endl;
    //    http_data_->setMInput(buf);
    //    http_data_->parse();
    //    ssize_t n = writen(fd, http_data_->get_m_output(), http_data_->get_output_len());
    //cout << n << endl;
  }
}


void SimpleForkServer::start() {
  cout << "Running..." << endl;
  while (true) {
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
    //cout << "running..." << endl;
  }
}

void SimpleForkServer::handle_write(int fd) {
  //  string buf = "HTTP/1.1 200 OK\r\nConnection: Close\r\nContent-Length: 11\r\nContent-type: text/plain\r\n\r\n";
  //  string buf1 = "Hello World";
}
