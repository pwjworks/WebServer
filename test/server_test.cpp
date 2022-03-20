//
// Created by pwjworks on 2022/2/9.
//


#include "SimpleEpollServer.h"
#include "SimpleForkServer.h"

int main() {

  auto server = std::make_shared<SimpleEpollServer>(12342);
  server->start();
  return 0;
}