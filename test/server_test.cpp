//
// Created by pwjworks on 2022/2/9.
//

#include "echo_server.h"
#include <gtest/gtest.h>

TEST(server_init, init) {
  auto server = std::make_shared<Server>(4080);
  std::cout << server->getListenFd() << std::endl;
}

TEST(server_init, error_port) {
  auto server = std::make_shared<Server>(65599);
  std::cout << server->getListenFd() << std::endl;
}

TEST(echo_server, start) {
  auto server = std::make_shared<echo_server>(4080);
  server->start();
}