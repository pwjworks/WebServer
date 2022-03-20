#pragma once
#include <Channel.h>
#include <cstring>
#include <map>
#include <memory>
#include <string>

/**
 * 服务器处理HTTP请求的可能结果
 */
enum class HTTP_CODE {
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
  STATE_WRITE,
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
 * 连接状态
 */
enum CONNNECTION_STATUS {
  H_CONNECTED = 0,
  H_DISCONNECTING,
  H_DISCONNECTED
};

struct cmp_str {
  bool operator()(char const *a, char const *b) const {
    return std::strcmp(a, b) < 0;
  }
};

class HttpData {
public:
  typedef std::shared_ptr<Channel> ChannelPtr;
  static const int INPUT_BUFFER_SIZE = 2048;
  static const int OUTPUT_BUFFER_SIZE = 1024;
  HttpData();
  ~HttpData();

  /**
   * 解析http请求一行数据
   * @return 提取状态
   */
  LINE_STATUS parse_line();
  /**
   * 解析http请求行,获取请求方法、目标URL，以及HTTP版本号
   * @return 是否获得Http请求行
   */
  bool parse_request_line(char *text);
  bool parse_headers(char *text);
  void parse_content(char *text);
  /**
   * 解析http请求
   * @return http响应
   */
  void parse();
  void analysis_request();
  void handle_read();
  void handle_write();
  void handle_conn();
  void handle_error();
  /**
   * 重置成员变量至初始状态
   */
  void reset();
  /**
   * 获取一行数据
   * @return
   */
  char *get_line() { return m_input_ + m_start_line; };
  void m_memcpy(char *text, int n);
  void m_memcpy(char *text);
  ChannelPtr get_channel() { return channel_; }
  void set_fd_(int fd) { fd_ = fd; }
  HTTP_CODE getHttpCode() const;
  void setMInput(char *mInput);

private:
  // channel指针
  ChannelPtr channel_;
  // 输入字符串
  char *m_input_;
  // 输出字符串
  char *m_output_;
  // HTTP请求URL
  char *m_url_;
  // HTTP请求版本
  char *m_version_;
  CONNNECTION_STATUS connection_state_;
  // 请求方法
  METHOD m_method;
  // 主状态机当前所处的状态
  PROCESS_STATE process_state_;

  int fd_;
  // 当前正在分析的字符在读缓冲区中的位置
  int m_checked_idx;
  // 正在解析的行的起始位置
  int m_start_line;
  // HTTP请求的消息体长度
  int m_content_length;
  int m_write_idx;
  bool keepAlive_;
  std::map<char *, char *, cmp_str> headers_;
  HTTP_CODE http_code_;


private:
};
