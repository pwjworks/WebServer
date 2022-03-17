#include "HttpData.h"
#include <cstring>

using namespace std;


LINE_STATUS HttpData::parse_line() {
  // 当前检查的字符
  char tmp;
  for (; m_checked_idx < m_read_idx; ++m_checked_idx) {
    tmp = m_input_[m_checked_idx];
    if (tmp == '\r') {
      // 当前字符为已读的最后一个字符
      if ((m_checked_idx + 1) == m_read_idx) {
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


HTTP_CODE HttpData::parse_request_line(char *text) {
  m_url_ = strpbrk(text, " \t");
  // 没空格的必然是错误请求
  if (!m_url_) return HTTP_CODE::BAD_REQUEST;
  *m_url_++ = '\0';
  char *method = text;
  if (strcasecmp(method, "GET") == 0) {
    m_method = METHOD::GET;
  } else if (strcasecmp(method, "POST") == 0) {
    m_method = METHOD::POST;
  } else {
    return HTTP_CODE::BAD_REQUEST;
  }
  // 去除多余空格
  m_url_ += strspn(m_url_, " \t");

  // 找到下一个空格，如果不存在则是错误请求
  m_version_ = strpbrk(m_url_, " \t");
  if (!m_version_) return HTTP_CODE::BAD_REQUEST;
  *m_version_++ = '\0';
  // 去除前面的空格
  m_version_ += strspn(m_url_, " \t");
  // 解析HTTP版本
  if (strcasecmp(m_version_, "HTTP/1.1") != 0) return HTTP_CODE::BAD_REQUEST;
  // 检查URL是否合法
  if (strncasecmp(m_url_, "http://", 7) == 0) {
    m_url_ += 7;
    m_url_ = strchr(m_url_, '/');
  }
  if (!m_url_ || m_url_[0] != '/') return HTTP_CODE::BAD_REQUEST;
  // 主状态机状态迁移
  process_state_ = PROCESS_STATE::STATE_PARSE_HEADERS;
  return HTTP_CODE::NO_REQUEST;
}

HEADER_STAUS HttpData::parse_headers(char *text) {
  HEADER_STAUS ret = HEADER_STAUS::HEADER_BAD;
  // 遇到空行，表示头部字段解析完毕
  if (text[0] == '\0') {
    if (m_content_length != 0) {
      process_state_ = PROCESS_STATE::STATE_PARSE_CONTENT;
    }
    return HEADER_STAUS::HEADER_OK;
  }
  do {
    char *key_ = text;
    char *value_ = nullptr;

    // 不存在冒号则为错误请求
    value_ = strpbrk(key_, " : ");
    // 无value则返回错误请求
    if (!value_) break;
    *value_++ = '\0';
    value_ += strspn(value_, " : ");
    if (strlen(value_) == 0) {
      break;
    } else {
      headers_[key_] = value_;
      ret = HEADER_STAUS::HEADER_OK;
    }
  } while (false);
  return ret;
}


HTTP_CODE HttpData::parse() {
  LINE_STATUS line_status = LINE_STATUS::LINE_OK;
  HTTP_CODE ret = HTTP_CODE::NO_REQUEST;
  char *text = nullptr;
  while (((process_state_ != PROCESS_STATE::STATE_FINISH) && (line_status == LINE_STATUS::LINE_OK)) || ((line_status = parse_line()) == LINE_STATUS::LINE_OK)) {
    text = get_line();
    m_start_line = m_checked_idx;
    switch (process_state_) {
      case PROCESS_STATE::STATE_PARSE_REQUESTLINE: {
        ret = parse_request_line(text);
        if (ret == HTTP_CODE::BAD_REQUEST) {
          return HTTP_CODE::BAD_REQUEST;
        }
        break;
      }
      case PROCESS_STATE::STATE_PARSE_HEADERS: {
        HEADER_STAUS header_status = parse_headers(text);
        if (header_status == HEADER_STAUS::HEADER_BAD) return HTTP_CODE::BAD_REQUEST;
        break;
      }
      case PROCESS_STATE::STATE_PARSE_CONTENT:
        break;
      case PROCESS_STATE::STATE_FINISH:
        break;
    }
  }
}


void HttpData::reset() {
  m_checked_idx = 0;
  m_read_idx = 0;
  m_start_line = 0;
  m_input_ = new char[INPUT_BUFFER_SIZE];
  m_output_ = new char[OUTPUT_BUFFER_SIZE];
  memset(m_input_, '\0', INPUT_BUFFER_SIZE);
  memset(m_output_, '\0', OUTPUT_BUFFER_SIZE);
}

void HttpData::setMInput(char *mInput) {
  reset();
  m_input_ = mInput;
  m_read_idx = strlen(m_input_);
}

HTTP_CODE HttpData::do_request() {
  return HTTP_CODE::NO_REQUEST;
}
HTTP_CODE HttpData::parse_content(char *text) {
  return HTTP_CODE::NO_REQUEST;
}
