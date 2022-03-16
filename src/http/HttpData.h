#pragma once
#include <string>

enum class HTTP_CODE {
  NO_REQUEST,
  GET_REQUEST,
  BAD_REQUEST,
  NO_RESOURCE,
  FORBIDDEN_REQUEST,
  FILE_REQUEST,
  INTERNAL_ERROR,
  CLOSED_CONNECTION
};


enum class ProcessState {
  STATE_PARSE_METHOD = 1,
  STATE_PARSE_URI,
  STATE_PARSE_VERSION,
  STATE_PARSE_HEADERS,

  STATE_RECV_BODY,
  STATE_ANALYSIS,
  STATE_ERROR,
  STATE_FINISH
};

enum class LINE_STATUS {
  LINE_OK = 0,
  LINE_BAD,
  LINE_OPEN
};

class HttpData {
public:
  static const int INPUT_BUFFER_SIZE = 2048;
  static const int OUTPUT_BUFFER_SIZE = 1024;
  HttpData() = default;
  ~HttpData() = default;

  /**
   * 提取http请求一行数据
   * @return 提取状态
   */
  LINE_STATUS parse_line();
  /**
   * 解析http请求行的请求行。
   * @return Http请求方法的解析状态
   */
  HTTP_CODE parse_request_line();
  /**
   * 去除请求的空格
   */
  void parse_whitespace();
  /**
   * 解析主方法
   */
  void parse();

  //  void handle_reponse();
  //  void handle_error();
  /**
   * 重置成员变量至初始状态
   */
  void reset();

  void setMInput(char *mInput);

private:
  char *m_input_;
  char *m_output_;
  char *m_url_;

private:
  // 表示读缓冲中已经读入的客户数据最后一个字节的下一个位置
  int m_read_idx;
  // 当前正在分析的字符在读缓冲区中的位置
  int m_checked_idx;
  // 正在解析的行的起始位置
  int m_start_line;

  ProcessState process_state_{ProcessState::STATE_PARSE_METHOD};
};
