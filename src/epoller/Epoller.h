#include <sys/epoll.h>
#include <vector>

class Epoller {
public:
  Epoller();
  ~Epoller() = default;

  void epoll_add(const int &fd, __uint32_t fd_events_, int timeout);
  void epoll_mod(const int &fd, __uint32_t fd_events_, int timeout);
  void epoll_del(const int &fd);

  int poll();

public:
  const std::vector<epoll_event> &getEvents() const;

private:
  int epollfd_;
  std::vector<epoll_event> events_;
};
