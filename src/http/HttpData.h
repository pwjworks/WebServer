#pragma once
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
  static const int INPUT_BUFFER_SIZE = 1024;
  static const int OUTPUT_BUFFER_SIZE = 1024;
  explicit HttpData(int fd);
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
  /**
   * 分析请求头并构建HTTP响应。
   */
  void analysis_request();
  /**
   * 读取请求内容，如果成功读取，则解析http报文，并构建HTTP响应。
   */
  void handle_read();
  /**
   * 将输出缓冲区的内容发送出去。
   */
  void handle_write();
  /**
   * 处理连接并重置成员变量，如果是长连接则保持连接。
   */
  void handle_conn();
  /**
   * 根据响应的HTTP代码构建错误响应报文。
   */
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


  void set_fd_(int fd) { fd_ = fd; }
  void setMInput(char *mInput);

  /**
   * 将响应内容复制到输出缓冲区。
   * @param text 响应内容
   * @param n 响应内容长度
   */
  void m_memcpy(char *text, int n);
  /**
   * 将响应内容复制到输出缓冲区，自动检测内容长度。
   * @param text 响应内容
   */
  void m_memcpy(char *text);

private:
  // 输入字符串
  char *m_input_;
  // 输出字符串
  char *m_output_;
  // HTTP请求URL
  char *m_url_{};
  // HTTP请求版本
  char *m_version_{};

  // 监听的socket
  int fd_;
  // 当前正在分析的字符在读缓冲区中的位置
  int m_checked_idx{};
  // 正在解析的行的起始位置
  int m_start_line{};
  // HTTP请求的消息体长度
  int m_content_length{};
  // 输出缓冲区的已写长度
  int m_write_idx{};
  // 是否为长连接
  bool keepAlive_{};

  std::map<char *, char *, cmp_str> headers_;
  CONNNECTION_STATUS connection_state_;
  // 请求方法
  METHOD m_method;
  // 主状态机当前所处的状态
  PROCESS_STATE process_state_;
  // HTTP响应代码
  HTTP_CODE http_code_;
};
