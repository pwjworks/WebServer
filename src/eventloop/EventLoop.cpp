#include "EventLoop.h"
#include "Util.h"
#include <cassert>
#include <sys/eventfd.h>
#include <unistd.h>

int createEventfd() {
  int evtfd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
  if (evtfd < 0) {
    abort();
  }
  return evtfd;
}

EventLoop::EventLoop(int listenfd) : looping_(false),
                                     quit_(false),
                                     wakeup_fd_(createEventfd()),
                                     epoller_ptr(std::make_shared<Epoller>(listenfd, wakeup_fd_)) {
  epoller_ptr->epoll_add(wakeup_fd_, EPOLLIN | EPOLLET, 1000);
}

EventLoop::~EventLoop() {
  close(wakeup_fd_);
}

void EventLoop::wakeup() {
  uint64_t one = 1;
  ssize_t n = writen(wakeup_fd_, (char *) (&one), sizeof one);
  if (n != sizeof one) {
  }
}

void EventLoop::loop() {
  assert(!looping_);
  looping_ = true;
  quit_ = false;
  while (!quit_) {
    event_handling_ = true;
    epoller_ptr->poll();
    event_handling_ = false;
  }
  looping_ = false;
}

void EventLoop::quit() {
  quit_ = true;
  wakeup();
}
