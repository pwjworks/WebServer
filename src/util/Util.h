#include <string>


ssize_t readn(int fd, void *buff, size_t n, bool &zero);
ssize_t writen(int fd, void *buff, size_t n);
/**
   * 设置非阻塞socket
   * @param fd 需要设置为非阻塞的socket
   * @return 是否修改成功的布尔值
   */
bool set_nonblock(int fd);
