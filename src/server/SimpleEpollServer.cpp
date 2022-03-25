#include "SimpleEpollServer.h"
#include "Util.h"
#include <cassert>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>

using namespace std;

SimpleEpollServer::SimpleEpollServer(int port) : Server(port),
                                                 epoller_(make_shared<Epoller>()),
                                                 accept_channel_(make_shared<Channel>(listenfd_)) {
  assert(set_nonblock(listenfd_) == true);
  accept_channel_->setFd(listenfd_);
  accept_channel_->set_events(EPOLLIN | EPOLLET);
  accept_channel_->set_read_callback([this] { handle_new_conn(); });
  // 把监听文件描述符加入epoller中
  epoller_->epoll_add(accept_channel_, 1000);
}


void SimpleEpollServer::start() {


  cout << "Running..." << endl;
  while (true) {
    std::vector<ChannelPtr> ret = epoller_->poll();
    for (auto &channel : ret) {
      channel->handle_event();
    }
  }
}
void SimpleEpollServer::handle_events(int fd) {
}

void SimpleEpollServer::handle_new_conn() {
  int clnt_fd = accept_new_conn();
  shared_ptr<HttpData> req_info = make_shared<HttpData>(clnt_fd);
  req_info->get_channel()->set_holder(req_info);
  req_info->get_channel()->set_events(EPOLLIN | EPOLLET);
  epoller_->epoll_add(req_info->get_channel(), 1000);
  // 设置非阻塞客户端socket
  if (!set_nonblock(clnt_fd)) close(clnt_fd);
}
