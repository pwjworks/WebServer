//
// Created by pwjworks on 2022/2/9.
//


#include "SimpleEpollServer.h"

int main() {
  auto server = std::make_shared<SimpleEpollServer>(12340);
  server->start();
  return 0;
}