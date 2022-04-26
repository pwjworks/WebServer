#include "HttpData.h"
#include "Util.h"
#include <cstring>

using namespace std;

HttpData::HttpData(int fd) : fd_(fd) {
  reset();
}
HttpData::~HttpData() {
}


LINE_STATUS HttpData::parse_line() {
  // 当前检查的字符
  char tmp;
  char *input = read_buff_.Peek();
  for (; m_checked_idx < m_content_length; ++m_checked_idx) {
    tmp = input[m_checked_idx];
    if (tmp == '\r') {
      // 当前字符为已读的最后一个字符
      if ((m_checked_idx + 1) == m_content_length) {
        return LINE_STATUS::LINE_OPEN;
      } else if (input[m_checked_idx + 1] == '\n') {// 检测到行结尾
        input[m_checked_idx++] = '\0';
        input[m_checked_idx++] = '\0';
        return LINE_STATUS::LINE_OK;
      }
      return LINE_STATUS::LINE_OPEN;
    } else if (tmp == '\n') {
      if ((m_checked_idx > 1) && input[m_checked_idx - 1] == '\r') {// 检测到行结尾，同时上一个字符为'\r'
        input[m_checked_idx - 1] = '\0';
        input[m_checked_idx++] = '\0';
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
      process_state_ = PROCESS_STATE::STATE_FINISH;
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
         (lineStatus == LINE_STATUS::LINE_OPEN && process_state_ == PROCESS_STATE::STATE_PARSE_CONTENT)) {
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
      case PROCESS_STATE::STATE_FINISH: {
        break;
      }
    }
  }
}


void HttpData::reset() {
  m_url_ = nullptr;
  m_version_ = nullptr;
  m_write_idx = 0;
  m_checked_idx = 0;
  m_content_length = 0;
  m_start_line = 0;
  keepAlive_ = false;
  headers_.clear();
  m_method = METHOD::GET;
  process_state_ = PROCESS_STATE::STATE_PARSE_REQUESTLINE;
  http_code_ = HTTP_CODE::BAD_REQUEST;
  write_buff_.RetrieveAll();
  read_buff_.RetrieveAll();
}

void HttpData::parse_content(char *text) {
  printf("content\n");
  m_memcpy(text);
  // 主状态机状态迁移
  process_state_ = PROCESS_STATE::STATE_FINISH;
}


void HttpData::handle_read() {
  ssize_t len = -1;
  int err_ = 0;
  len = read_buff_.ReadFd(fd_, &err_);
  if (len <= 0) {
    return;
  } else {
    m_content_length += len;
  }
}

void HttpData::analysis_request() {
  write_buff_.Append("HTTP/1.1 200 OK\r\n", 17);
  if (headers_.find("Connection") != headers_.end() && (strcasecmp(headers_["Connection"], "keep-alive") == 0)) {
    keepAlive_ = true;
    // 字符串拼接
    write_buff_.Append("Connection: keep-alive\r\nKeep-Alive: timeout=300000\r\n", 52);
  }
  write_buff_.Append("Content-Type: text/html; charset=utf-8\r\n\r\n<h1>Hello world!</h1><h3>Your headers:</h3>", 85);
  for (auto &kv : headers_) {
    write_buff_.Append("<div>", 5);
    write_buff_.Append(kv.first, strlen(kv.first));
    write_buff_.Append(":", 1);
    write_buff_.Append(kv.second, strlen(kv.second));
    write_buff_.Append("</div>", 6);
  }
}

void HttpData::handle_write() {
  write_buff_.WriteFd(fd_, nullptr);
}

void HttpData::handle_conn() {
  close(fd_);
  reset();
}

void HttpData::handle_error() {
  switch (http_code_) {
    case HTTP_CODE::GET_REQUEST:
      break;
    case HTTP_CODE::BAD_REQUEST:
      break;
    case HTTP_CODE::NO_RESOURCE:
      break;
    case HTTP_CODE::FORBIDDEN_REQUEST:
      break;
    case HTTP_CODE::FILE_REQUEST:
      break;
    case HTTP_CODE::INTERNAL_ERROR:
      break;
    case HTTP_CODE::CLOSED_CONNECTION:
      break;
  }
}

void HttpData::m_memcpy(char *text, int n) {
  memcpy(m_output_ + m_write_idx, text, n);
  m_write_idx += n;
}

void HttpData::m_memcpy(char *text) {
  int size = strlen(text);
  memcpy(m_output_ + m_write_idx, text, size);
  m_write_idx += size;
}

void HttpData::setMInput(char *mInput) {
  reset();
  m_content_length = strlen(mInput);
  memcpy(m_input_, mInput, m_content_length);
}
