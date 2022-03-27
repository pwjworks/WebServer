#include "SimpleEpollServer.h"
#include "Util.h"
#include <cassert>
#include <iostream>

using namespace std;

SimpleEpollServer::SimpleEpollServer(int port) : Server(port),
                                                 eventloop_ptr_(make_shared<EventLoop>(listenfd_)) {
  assert(set_nonblock(listenfd_) == true);
  eventloop_ptr_->addToPoller(listenfd_, EPOLLIN | EPOLLET);
}

void SimpleEpollServer::start() {
  cout << "Running..." << endl;
  eventloop_ptr_->loop();
}
