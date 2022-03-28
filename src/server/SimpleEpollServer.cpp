#include "SimpleEpollServer.h"
#include "Util.h"
#include <cassert>
#include <iostream>
#include <unistd.h>

using namespace std;

SimpleEpollServer::SimpleEpollServer(int port) : Server(port),
                                                 threadpool_ptr_(make_shared<EventLoopThreadPool>()),
                                                 quit_(false) {
  assert(set_nonblock(listenfd_) == true);
  threadpool_ptr_->init(16);
}

void SimpleEpollServer::start() {
  cout << "Running..." << endl;
  threadpool_ptr_->start();
  int accept_fd = -1;
  while (!quit_) {
    accept_fd = accept_new_conn();
    if (accept_fd > 0 && accept_fd < 1000 && set_nonblock(accept_fd)) {
      threadpool_ptr_->add_fd(accept_fd);
      //close(accept_fd);
    }
  }
}
