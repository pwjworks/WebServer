//
// Created by pwjworks on 2022/2/9.
//


#include "Server.h"

int main() {
  auto server = std::make_shared<Server>(12345);
  server->start();
  return 0;
}