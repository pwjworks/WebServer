#include "Epoller.h"
#include "HttpData.h"
#include <memory>
#include <sys/epoll.h>
#include <vector>


class Server {
public:
  typedef std::shared_ptr<Epoller> EpollerPtr;
  typedef std::shared_ptr<HttpData> HttpDataPtr;
  explicit Server(int port);
  ~Server();

  /**
   * 设置非阻塞socket
   * @param fd 需要设置为非阻塞的socket
   * @return 是否修改成功的布尔值
   */
  bool set_nonblock(int fd);
  /**
   * 初始化服务器socket
   * @param port 服务器监听端口
   * @return 是否初始化成功的布尔值
   */
  bool server_fd_init(int port);
  /**
   * 接收新连接
   * @return 是否接受成功
   */
  bool accept_new_conn();
  void handle_read(int fd);
  void handle_write(int fd);
  void start();

private:
  EpollerPtr epoller_;
  HttpDataPtr http_data_;
  int listenfd_;
};
