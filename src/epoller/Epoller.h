#include "HttpData.h"
#include <memory>
#include <sys/epoll.h>
#include <vector>

class Epoller {
public:
  Epoller(int, int);
  Epoller();
  ~Epoller() = default;
  using HttpDataPtr = std::shared_ptr<HttpData>;
  void epoll_add(const int &fd, __uint32_t fd_events_, int timeout);
  void epoll_mod(const int &fd, __uint32_t fd_events_, int timeout);
  void epoll_del(const int &fd);
  void handle_event(int fd, __uint32_t revents_);
  int handle_new_conn();

  void poll();

public:
  const std::vector<epoll_event> &getEvents() const;

private:
  static const int MAXFDS = 100000;
  int wakeupfd_;
  int listenfd_;
  int epollfd_;
  std::vector<epoll_event> events_;
  std::shared_ptr<HttpData> fd2http_[MAXFDS];
};
