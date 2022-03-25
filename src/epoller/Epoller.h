#include "Channel.h"
#include "HttpData.h"
#include <memory>
#include <sys/epoll.h>
#include <vector>

class Epoller {
public:
  typedef std::shared_ptr<Channel> ChannelPtr;
  typedef std::shared_ptr<HttpData> HttpDataPtr;
  Epoller();
  ~Epoller() = default;

  void epoll_add(ChannelPtr request, int timeout);
  void epoll_mod(ChannelPtr request, int timeout);
  void epoll_del(ChannelPtr request);
  std::vector<ChannelPtr> get_events_request(int events_num);
  std::vector<ChannelPtr> poll();

  const std::vector<epoll_event> &getEvents() const;

private:
  static const int MAXFDS = 100000;
  int epollfd_;
  std::vector<epoll_event> events_;
  ChannelPtr fd2chan_[MAXFDS];
  HttpDataPtr fd2http_[MAXFDS];
};
