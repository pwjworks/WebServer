#include "util.h"
#include <unistd.h>

using namespace std;

ssize_t writen(int fd, string &sbuff) {
  size_t nleft = sbuff.size();
  ssize_t nwritten = 0;
  ssize_t writeSum = 0;
  const char *ptr = sbuff.c_str();
  while (nleft > 0) {
    if ((nwritten = write(fd, ptr, nleft)) <= 0) {
      if (nwritten < 0) {
        if (errno == EINTR) {
          nwritten = 0;
          continue;
        } else if (errno == EAGAIN)
          break;
        else
          return -1;
      }
    }
    writeSum += nwritten;
    nleft -= nwritten;
    ptr += nwritten;
  }
  if (writeSum == static_cast<int>(sbuff.size()))
    sbuff.clear();
  else
    sbuff = sbuff.substr(writeSum);
  return writeSum;
}