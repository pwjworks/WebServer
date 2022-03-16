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
      if ((m_checked_idx + 1) == m_checked_idx) {
        return LINE_STATUS::LINE_OPEN;
      } else if (m_input_[m_checked_idx++] == '\n') {// 检测到行结尾
        m_input_[m_checked_idx - 1] = '\0';
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
      return LINE_STATUS::LINE_OPEN;
    }
  }
  return LINE_STATUS::LINE_OPEN;
}

HTTP_CODE HttpData::parse_request_line(char *text) {
  m_url_ = strpbrk(text, " \t");
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

  m_url_ += strspn(m_url_, " \t");
  m_version_ = strpbrk(m_url_, " \t");
  if (!m_version_) return HTTP_CODE::BAD_REQUEST;
  *m_version_++ = '\0';
  m_version_ += strspn(m_url_, " \t");
  // 解析HTTP版本
  if (strcasecmp(m_version_, "HTTP/1.1") != 0) return HTTP_CODE::BAD_REQUEST;
  // 检查URL是否合法
  if (strncasecmp(m_url_, "http://", 7) == 0) {
    m_url_ += 7;
    m_url_ = strchr(m_url_, '/');
  }
  if (!m_url_ || m_url_[0] != '/') return HTTP_CODE::BAD_REQUEST;
  process_state_ = PROCESS_STATE::STATE_PARSE_HEADERS;
  return HTTP_CODE::NO_REQUEST;
}

void HttpData::parse() {
  LINE_STATUS line_status = LINE_STATUS::LINE_OK;
  HTTP_CODE res = HTTP_CODE::NO_REQUEST;
  char *text = nullptr;
  while (((process_state_ == PROCESS_STATE::STATE_FINISH) && (line_status == LINE_STATUS::LINE_OK)) || ((line_status = parse_line()) == LINE_STATUS::LINE_OK)) {
    text = get_line();
  }
}

void HttpData::reset() {
  m_checked_idx = 0;
  m_read_idx = 0;
  m_start_line = 0;
  memset(m_input_, '\0', INPUT_BUFFER_SIZE);
  memset(m_output_, '\0', OUTPUT_BUFFER_SIZE);
}

void HttpData::setMInput(char *mInput) {
  reset();
  m_input_ = mInput;
}
