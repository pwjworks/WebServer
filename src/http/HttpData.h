#pragma once
#include <string>

enum class ProcessState {
  STATE_PARSE_METHOD = 1,
  STATE_PARSE_URI,
  STATE_PARSE_HEADERS,
  STATE_RECV_BODY,
  STATE_ANALYSIS,
  STATE_ERROR,
  STATE_FINISH
};

enum class URIState {
  PARSE_URI_ERROR = 1,
  PARSE_URI_SUCCESS,
};

enum HttpMethod {
  METHOD_POST = 1,
  METHOD_GET,
  METHOD_UNIMPLEMENTED
};

class HttpData {
public:
  HttpData() = default;
  ~HttpData() = default;

  /**
   * 解析请求URI
   * @return Http请求URI的解析状态
   */
  URIState parse_URI();
  /**
   * 解析http请求行的请求方法。
   * @return Http请求方法的解析状态
   */
  HttpMethod parse_method();
  /**
   * 去除请求的空格
   */
  void parse_whitespace();
  /**
   * 解析主方法
   */
  void parse();

  void handle_reponse();
  void handle_error();
  /**
   * 重置成员变量至初始状态
   */
  void reset();

public:
  void setInBuffer(const std::string &inBuffer);
  const std::string &getOutBuffer() const;

private:
  std::string in_buffer_;
  std::string out_buffer_;


private:
  int read_pos_{0};
  size_t size_{0};

  URIState uri_state_{URIState::PARSE_URI_ERROR};
  HttpMethod method_{HttpMethod::METHOD_UNIMPLEMENTED};
  ProcessState process_state_{ProcessState::STATE_PARSE_METHOD};
};
