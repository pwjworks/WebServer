#pragma once
#include <map>
#include <string>

/**
 * 服务器处理HTTP请求的可能结果
 */
enum class HTTP_CODE {
  NO_REQUEST,       // 请求不完整，需要继续解析客户端数据
  GET_REQUEST,      // 获得了一个完整的客户请求
  BAD_REQUEST,      // 客户请求有语法错误
  NO_RESOURCE,      // 客户请求不存在的资源
  FORBIDDEN_REQUEST,// 客户端没有权限请求该资源
  FILE_REQUEST,     // 文件请求
  INTERNAL_ERROR,   // 服务器内部错误
  CLOSED_CONNECTION // 客户端已经关闭连接
};

/**
 * 主状态机
 */
enum class PROCESS_STATE {
  STATE_PARSE_REQUESTLINE = 1,
  STATE_PARSE_HEADERS,
  STATE_PARSE_CONTENT,
  STATE_FINISH
};

/**
 * HTTP请求方法，仅支持GET和POST方法。
 */
enum class METHOD {
  GET = 0,
  POST,
  HEAD,
  PUT,
  DELETE,
  TRACE,
  OPTIONS,
  CONNECT,
  PATCH
};

/**
 * 从状态机，行的读取状态
 */
enum class LINE_STATUS {
  LINE_OK = 0,// 读取到一个完整的行
  LINE_BAD,   // 行出错
  LINE_OPEN   // 行数据尚且不完整
};

/**
 * 从状态机，header的读取状态
 */
enum class HEADER_STAUS {
  HEADER_OK = 0,// 读取到一个header
  HEADER_BAD    // header出错
};

class HttpData {
public:
  static const int INPUT_BUFFER_SIZE = 2048;
  static const int OUTPUT_BUFFER_SIZE = 1024;
  HttpData();
  ~HttpData() = default;

  /**
   * 解析http请求一行数据
   * @return 提取状态
   */
  LINE_STATUS parse_line();
  /**
   * 解析http请求行,获取请求方法、目标URL，以及HTTP版本号
   * @return Http请求方法的解析状态
   */
  HTTP_CODE parse_request_line(char *text);
  HEADER_STAUS parse_headers(char *text);
  HTTP_CODE parse_content(char *text);
  /**
   *
   * @return
   */
  HTTP_CODE parse();

  HTTP_CODE do_request();
  //  void handle_error();
  /**
   * 重置成员变量至初始状态
   */
  void reset();
  /**
   * 获取一行数据
   * @return
   */
  char *get_line() { return m_input_ + m_start_line; };

  void setMInput(char *mInput);
  char *get_m_output();
  int get_output_len();


private:
  // 输入字符串
  char *m_input_;
  // 输出字符串
  char *m_output_;


private:
  // HTTP请求URL
  char *m_url_;
  // HTTP请求版本
  char *m_version_;

private:
  // 表示读缓冲中已经读入的客户数据最后一个字节的下一个位置
  int m_read_idx;
  // 当前正在分析的字符在读缓冲区中的位置
  int m_checked_idx;
  // 正在解析的行的起始位置
  int m_start_line;
  // HTTP请求的消息体长度
  int m_content_length;
  std::map<char *, char *> headers_;
  // 请求方法
  METHOD m_method;
  // 主状态机当前所处的状态
  PROCESS_STATE process_state_;
};
