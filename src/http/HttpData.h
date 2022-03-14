#pragma once
#include <string>

enum class ProcessState {
  STATE_PARSE_METHOD = 1,
  STATE_PARSE_URI,
  STATE_PARSE_HEADERS,
  STATE_RECV_BODY,
  STATE_ANALYSIS,
  STATE_FINISH
};

enum class URIState {
  PARSE_URI_AGAIN = 1,
  PARSE_URI_ERROR,
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

  void parseURI();
  /**
   * 解析http请求行的请求方法。
   * @return Http请求方法
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

  void handle_error();

public:
  void
  setInBuffer(const std::string &inBuffer);

private:
  std::string in_buffer_;
  int read_pos_{0};
  size_t size_{0};
  HttpMethod method_;
  ProcessState processState_;
};
