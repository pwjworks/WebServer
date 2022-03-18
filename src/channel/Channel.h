#pragma once

#include <functional>

class Channel {
public:
  typedef std::function<void()> EventCallback;

  Channel();
  ~Channel();

private:
  const int fd_;
  int events_;
  int revents_;
  int index_;
  bool logHup_;
};
