#pragma once

#include <functional>

class Channel {
public:
  typedef std::function<void()> EventCallback;
  typedef std::function<void()> ReadEventCallback;

  Channel();
  ~Channel();

  void handle_event();
  void set_read_callback(ReadEventCallback cb) { readCallback_ = std::move(cb); }
  void set_write_callback(EventCallback cb) { writeCallback_ = std::move(cb); }
  void set_conn_callback(EventCallback cb) { connCallback_ = std::move(cb); }
  void set_error_callback(EventCallback cb) { errorCallback_ = std::move(cb); }

  void handle_read() {
    if (readCallback_) readCallback_();
  }
  void handle_write() {
    if (writeCallback_) writeCallback_();
  }
  void handle_conn() {
    if (connCallback_) connCallback_();
  }
  void handle_error() {
    if (errorCallback_) errorCallback_();
  }

  void set_revents(__uint32_t ev) { revents_ = ev; }

  void set_events(__uint32_t ev) { events_ = ev; }
  __uint32_t &get_events() { return events_; }
  int get_fd() const { return fd_; };
  void setFd(int fd) { fd_ = fd; };

private:
  ReadEventCallback readCallback_;
  EventCallback writeCallback_;
  EventCallback connCallback_;
  EventCallback errorCallback_;

  int fd_;
  __uint32_t events_;
  __uint32_t revents_;
};
