#include "SimpleForkServer.h"
#include "Util.h"
#include <iostream>
#include <netinet/in.h>
#include <sys/wait.h>
#include <unistd.h>

using namespace std;

SimpleForkServer::SimpleForkServer(int port) : Server(port) {
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
      handle_events(clint_fd);
      close(clint_fd);
      // 处理完毕后立即退出进程
      break;
    } else {
      int stat;
      waitpid(-1, &stat, WNOHANG);
      close(clint_fd);
    }
  }
}
void SimpleForkServer::handle_events(int fd) {
  cout << fd << endl;
  http_data_->set_fd_(fd);
  http_data_->handle_read();
  http_data_->handle_write();
  http_data_->handle_conn();
}
