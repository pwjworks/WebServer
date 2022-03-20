#include "HttpData.h"
#include "Util.h"
#include <cstring>
#include <unistd.h>

using namespace std;

HttpData::HttpData() {
  m_input_ = new char[INPUT_BUFFER_SIZE];
  m_output_ = new char[OUTPUT_BUFFER_SIZE];
  reset();
}
HttpData::~HttpData() {
}


LINE_STATUS HttpData::parse_line() {
  // 当前检查的字符
  char tmp;
  for (; m_checked_idx < m_content_length; ++m_checked_idx) {
    tmp = m_input_[m_checked_idx];
    if (tmp == '\r') {
      // 当前字符为已读的最后一个字符
      if ((m_checked_idx + 1) == m_content_length) {
        return LINE_STATUS::LINE_OPEN;
      } else if (m_input_[m_checked_idx + 1] == '\n') {// 检测到行结尾
        m_input_[m_checked_idx++] = '\0';
        m_input_[m_checked_idx++] = '\0';
        return LINE_STATUS::LINE_OK;
      }
      return LINE_STATUS::LINE_OPEN;
    } else if (tmp == '\n') {
      if ((m_checked_idx > 1) && m_input_[m_checked_idx - 1] == '\r') {// 检测到行结尾，同时上一个字符为'\r'
        m_input_[m_checked_idx - 1] = '\0';
        m_input_[m_checked_idx++] = '\0';
        return LINE_STATUS::LINE_OK;
      }
      return LINE_STATUS::LINE_BAD;
    }
  }
  return LINE_STATUS::LINE_OPEN;
}


bool HttpData::parse_request_line(char *text) {
  m_url_ = strpbrk(text, " \t");
  // 没空格的必然是错误请求
  if (!m_url_) return false;
  *m_url_++ = '\0';
  char *method = text;
  if (strcasecmp(method, "GET") == 0) {
    m_method = METHOD::GET;
  } else if (strcasecmp(method, "POST") == 0) {
    m_method = METHOD::POST;
  } else {
    // 未实现请求
    return false;
  }
  // 去除多余空格
  m_url_ += strspn(m_url_, " \t");

  // 找到下一个空格，如果不存在则是错误请求
  m_version_ = strpbrk(m_url_, " \t");
  if (!m_version_) return false;
  *m_version_++ = '\0';
  // 去除前面的空格
  m_version_ += strspn(m_url_, " \t");
  // 解析HTTP版本
  if (strcasecmp(m_version_, "HTTP/1.1") != 0) return false;
  // 检查URL是否合法
  if (strncasecmp(m_url_, "http://", 7) == 0) {
    m_url_ += 7;
    m_url_ = strchr(m_url_, '/');
  }
  if (!m_url_ || m_url_[0] != '/') return false;

  return true;
}

bool HttpData::parse_headers(char *text) {
  // 遇到空行，表示头部字段解析完毕
  if (text[0] == '\0') {
    http_code_ = HTTP_CODE::GET_REQUEST;
    analysis_request();
    if (m_content_length > m_checked_idx) {
      process_state_ = PROCESS_STATE::STATE_PARSE_CONTENT;
    } else {
      process_state_ = PROCESS_STATE::STATE_WRITE;
    }
    return true;
  }

  char *key_ = text;
  char *value_ = nullptr;

  // 不存在冒号则为错误请求
  value_ = strpbrk(key_, " : ");
  // 无value则返回错误请求
  if (!value_) return false;
  *value_++ = '\0';
  value_ += strspn(value_, " : ");
  if (strlen(value_) == 0) {
    return false;
  } else {
    // 解析请求参数完毕
    headers_[key_] = value_;
    return true;
  }
}


void HttpData::parse() {
  char *text;
  LINE_STATUS lineStatus;
  while ((lineStatus = parse_line()) == LINE_STATUS::LINE_OK ||
         (lineStatus == LINE_STATUS::LINE_OPEN && process_state_ == PROCESS_STATE::STATE_PARSE_CONTENT) ||
         process_state_ == PROCESS_STATE::STATE_WRITE) {
    text = get_line();
    m_start_line = m_checked_idx;
    switch (process_state_) {
      case PROCESS_STATE::STATE_PARSE_REQUESTLINE: {
        if (parse_request_line(text)) {
          // 主状态机状态迁移
          process_state_ = PROCESS_STATE::STATE_PARSE_HEADERS;
        } else {
          http_code_ = HTTP_CODE::BAD_REQUEST;
          // 返回错误
          handle_error();
          process_state_ = PROCESS_STATE::STATE_FINISH;
        }
        break;
      }
      case PROCESS_STATE::STATE_PARSE_HEADERS: {
        if (!parse_headers(text)) {
          http_code_ = HTTP_CODE::BAD_REQUEST;
          handle_error();
          process_state_ = PROCESS_STATE::STATE_FINISH;
        }

        break;
      }
        // 解析HTTP请求内容
      case PROCESS_STATE::STATE_PARSE_CONTENT: {

        parse_content(text);
        break;
      }
      case PROCESS_STATE::STATE_WRITE: {
        handle_write();
        break;
      }
      case PROCESS_STATE::STATE_FINISH: {

        break;
      }
    }
  }
}


void HttpData::reset() {
  m_checked_idx = 0;
  m_content_length = 0;
  m_start_line = 0;
  keepAlive_ = false;
  process_state_ = PROCESS_STATE::STATE_PARSE_REQUESTLINE;
  http_code_ = HTTP_CODE::BAD_REQUEST;
  headers_.clear();
}

void HttpData::parse_content(char *text) {
  printf("content\n");
  strcat(m_output_, text);
  // 主状态机状态迁移
  process_state_ = PROCESS_STATE::STATE_WRITE;
}


void HttpData::handle_read() {
  __uint32_t &events_ = channel_->get_events();
  do {
    bool zero = false;
    int read_num = readn(fd_, m_input_, INPUT_BUFFER_SIZE, zero);
    if (connection_state_ == CONNNECTION_STATUS::H_DISCONNECTING) {
      memset(m_input_, '\0', INPUT_BUFFER_SIZE);
      break;
    }
    if (read_num < 0) {
      handle_error();
      break;
    } else if (zero) {
      connection_state_ = CONNNECTION_STATUS::H_DISCONNECTING;
      if (read_num == 0) break;
    }
    m_content_length = read_num;
  } while (false);
}

void HttpData::analysis_request() {
  m_memcpy("HTTP/1.1 200 OK\r\n", 17);
  if (headers_.find("Connection") != headers_.end() && (strcasecmp(headers_["Connection"], "keep-alive") == 0)) {
    keepAlive_ = true;
    // 字符串拼接
    m_memcpy("Connection: keep-alive\r\nKeep-Alive: timeout=300000\r\n", 52);
  }
  m_memcpy("Content-Type: text/html; charset=utf-8\r\n\r\n<h1>Hello world!</h1>", 63);
}

void HttpData::handle_write() {
  //  printf("write!");

  if (connection_state_ != H_DISCONNECTED) {
    __uint32_t &events_ = channel_->get_events();
    int n = writen(fd_, m_output_, m_write_idx);
    printf("%d\n", n);
    if (n < 0) {
      events_ = 0;
    }
    //if (strlen(m_output_) > 0) events_ |= EPOLLOUT;
  }
  handle_conn();
  process_state_ = PROCESS_STATE::STATE_FINISH;
}

void HttpData::handle_conn() {
  memset(m_input_, '\0', INPUT_BUFFER_SIZE);
  memset(m_output_, '\0', OUTPUT_BUFFER_SIZE);
  reset();
  m_write_idx = 0;

  //  if (!keepAlive_)
  close(fd_);
}
void HttpData::handle_error() {
}

HTTP_CODE HttpData::getHttpCode() const {
  return http_code_;
}
void HttpData::m_memcpy(char *text, int n) {
  memcpy(m_output_ + m_write_idx, text, n);
  m_write_idx += n;
}

void HttpData::m_memcpy(char *text) {
  memset(m_input_, '\0', INPUT_BUFFER_SIZE);
  int size = strlen(text);
  memcpy(m_output_ + m_write_idx, text, size);
  m_write_idx += size;
}
void HttpData::setMInput(char *mInput) {
  memset(m_input_, '\0', m_content_length);
  reset();
  m_content_length = strlen(mInput);
  memcpy(m_input_, mInput, m_content_length);
}
