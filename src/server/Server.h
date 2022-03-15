#pragma once

class Server {
public:
  explicit Server(int port);
  virtual ~Server();


  /**
   * 初始化服务器socket
   * @param port 服务器监听端口
   * @return 是否初始化成功的布尔值
   */
  bool server_fd_init(int port);
  /**
   * 接收新连接
   * @return 客户端fd
   */
  int accept_new_conn();
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
