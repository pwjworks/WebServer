#include "Util.h"
#include <fcntl.h>
#include <iostream>
#include <unistd.h>

using namespace std;

ssize_t readn(int fd, void *buff, size_t n, bool &zero) {
  size_t nleft = n;
  ssize_t nread = 0;
  ssize_t readSum = 0;
  char *ptr = (char *) buff;
  while (nleft > 0) {
    if ((nread = read(fd, ptr, nleft)) < 0) {
      if (errno == EINTR) {
        nread = 0;
      } else if (errno == EAGAIN) {
        return readSum;
      } else {
        return -1;
      }
    } else if (nread == 0) {
      zero = true;
      break;
    }

    readSum += nread;
    nleft -= nread;
    ptr += nread;
  }
  return readSum;
}


ssize_t writen(int fd, void *buff, size_t n) {
  size_t nleft = n;
  ssize_t nwritten = 0;
  ssize_t writeSum = 0;
  char *ptr = (char *) buff;
  while (nleft > 0) {
    if ((nwritten = write(fd, ptr, nleft)) <= 0) {
      if (nwritten < 0) {
        if (errno == EINTR) {
          nwritten = 0;
          continue;
        } else if (errno == EAGAIN) {
          return writeSum;
        } else
          return -1;
      }
    }
    writeSum += nwritten;
    nleft -= nwritten;
    ptr += nwritten;
  }
  return writeSum;
}

bool set_nonblock(int fd) {
  int oldSocketFlag = fcntl(fd, F_GETFL, 0);
  int newSocketFlag = oldSocketFlag | O_NONBLOCK;
  if (fcntl(fd, F_SETFL, newSocketFlag) == -1) {
    cout << "set listenfd to nonblock error" << endl;
    return false;
  }
  return true;
}