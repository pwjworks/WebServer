#include "Epoller.h"
#include <memory>
#include <sys/epoll.h>
#include <vector>


class Server {
public:
  typedef std::shared_ptr<Epoller> Epoller_ptr;
  explicit Server(int port);
  ~Server();

  /**
   * 设置非阻塞描述符
   * @param fd 需要设置为非阻塞的描述符
   * @return 是否修改成功的布尔值
   */
  bool set_nonblock(int fd);
  bool server_fd_init(int port);
  bool accept_new_conn();
  void handle_read(int index);
  void start();

private:
  Epoller_ptr epoller_;
  int listenfd_;
};
