#include "Server.h"
#include <HttpData.h>
#include <memory>

#pragma once

class SimpleForkServer : public Server {
public:
  explicit SimpleForkServer(int port);
  ~SimpleForkServer() override = default;
  void handle_events(int fd) override;
  void start() override;
};
