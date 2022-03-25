#include "SimpleEpollServer.h"
#include "Util.h"
#include <cassert>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>

using namespace std;

SimpleEpollServer::SimpleEpollServer(int port) : Server(port),
                                                 epoller_(make_shared<Epoller>()),
                                                 accept_channel_(make_shared<Channel>()) {
  assert(set_nonblock(listenfd_) == true);
  accept_channel_->setFd(listenfd_);
  accept_channel_->set_read_callback([this] { handle_new_conn(); });
}


void SimpleEpollServer::start() {
  // 把监听文件描述符加入epoller中
  epoller_->epoll_add(listenfd_, EPOLLIN, 1000);

  cout << "Running..." << endl;

  while (true) {
    //cout << "Running..." << endl;
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
        revents = epoller_->getEvents()[i].events;
        if (curr_fd == listenfd_) {
          accept_channel_->set_revents(EPOLLIN | EPOLLET);
          accept_channel_->set_events(0);
          accept_channel_->handle_event();
        } else {
          handle_events(curr_fd);
        }
      }
    }
  }
}
void SimpleEpollServer::handle_events(int fd) {
  http_data_->set_fd_(fd);
  auto cp = http_data_->get_channel();
  cp->set_revents(revents);
  cp->setFd(fd);
  cp->handle_event();
}

void SimpleEpollServer::handle_new_conn() {
  int clnt_fd = accept_new_conn();
  epoller_->epoll_add(clnt_fd, EPOLLIN, 1000);
  // 设置非阻塞客户端socket
  if (!set_nonblock(clnt_fd)) close(clnt_fd);
}
