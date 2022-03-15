#include "Server.h"
#include <HttpData.h>
#include <memory>

#pragma once

class SimpleForkServer : public Server {
public:
  typedef std::shared_ptr<HttpData> HttpDataPtr;
  explicit SimpleForkServer(int port);

  void handle_read(int fd) override;
  void handle_write(int fd) override;
  void start() override;

private:
  HttpDataPtr http_data_;
};
