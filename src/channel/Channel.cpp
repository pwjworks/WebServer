#include "Channel.h"
#include <sys/epoll.h>

Channel::Channel() : events_(0), fd_(0) {
}

Channel::~Channel() {}

void Channel::handle_event() {
  events_ = 0;
  if ((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN)) {
    events_ = 0;
    return;
  }
  if (revents_ & EPOLLERR) {
    handle_error();
    events_ = 0;
    return;
  }
  if (revents_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)) {
    handle_read();
  }
  if (revents_ & EPOLLOUT) {
    handle_write();
  }
  handle_conn();
}
