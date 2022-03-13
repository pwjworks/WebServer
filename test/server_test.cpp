//
// Created by pwjworks on 2022/2/9.
//


#include "Server.h"
#include <memory>

int main() {
  auto server = std::make_shared<Server>(12345);
  server->Start();
  return 0;
}