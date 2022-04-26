//
// Created by pwjworks on 2022/2/9.
//


#include "ReactorServer.h"
#include "SimpleEpollServer.h"
#include "SimpleForkServer.h"

int main() {
  auto server = std::make_shared<ReactorServer>(12341);
  server->start();
  return 0;
}