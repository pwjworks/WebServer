//
// Created by pwjworks on 2022/2/8.
//
#pragma once
#include <sys/epoll.h>

class Epoller{
public:
    Epoller();
    ~Epoller();
    void epoll_add(int fd,int timeout);
    void epoll_mod(int fd,int timeout);
    void epoll_del(int fd);
private:
    static const int MAXFDS=100000;
    int epollFd_;


};

