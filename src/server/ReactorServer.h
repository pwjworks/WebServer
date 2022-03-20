#pragma once

class ReactorServer : public Server {
public:
  typedef std::shared_ptr<Epoller> EpollerPtr;
  typedef std::shared_ptr<HttpData> HttpDataPtr;

  explicit SimpleEpollServer(int port);
  ~SimpleEpollServer() override = default;

  void handle_read(int fd) override;
  void handle_write(int fd) override;
  void start() override;


private:
  EpollerPtr epoller_;
  HttpDataPtr http_data_;
};
