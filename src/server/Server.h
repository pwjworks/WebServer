#pragma once

class Server {
public:
  explicit Server(int port);
  virtual ~Server();

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
  virtual bool accept_new_conn() = 0;
  /**
   * 处理socket读事件
   * @param fd
   */
  virtual void handle_read(int fd) = 0;
  /**
   * 处理socket写事件
   * @param fd
   */
  virtual void handle_write(int fd) = 0;
  virtual void start() = 0;


protected:
  int listenfd_;
};
