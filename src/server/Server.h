#include <sys/epoll.h>
#include <vector>
class Server {
public:
  explicit Server(int port);
  ~Server();

  void epoll_add(const int &fd, __uint32_t fd_events_, int timeout);
  void epoll_mod(const int &fd, __uint32_t fd_events_, int timeout);
  void epoll_del(const int &fd);
  bool set_nonblock(int fd);
  bool server_fd_init(int port);
  bool accept_newconn();
  void handleRead(int index);
  void Start();

private:
  std::vector<epoll_event> events_;
  int listenfd_;
  int epollfd_;
};
