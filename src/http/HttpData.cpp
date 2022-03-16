#include "HttpData.h"
#include <cstring>

using namespace std;

HTTP_CODE HttpData::parse_request_line() {
  //  m_url_ = strpbrk(in_buffer_, " \t");
}

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

void HttpData::parse_whitespace() {
  //  while (read_pos_ < size_ && (in_buffer_[read_pos_] == ' ' || in_buffer_[read_pos_] == '\t' || in_buffer_[read_pos_] == '\n' || in_buffer_[read_pos_] == '\r'))
  //    read_pos_++;
}

void HttpData::parse() {
  //  do {
  //    // 解析请求方法
  //    parse_whitespace();
  //    method_ = parse_method();
  //    // 检查状态，出现错误则直接返回错误响应
  //    if (method_ != HttpMethod::METHOD_UNIMPLEMENTED) {
  //      process_state_ = ProcessState::STATE_PARSE_URI;
  //    } else {
  //      process_state_ = ProcessState::STATE_ERROR;
  //      break;
  //    }
  //
  //    if (process_state_ != ProcessState::STATE_PARSE_URI) break;
  //    // 解析请求URI
  //    parse_whitespace();
  //    uri_state_ = parse_URI();
  //    // 检查状态，出现错误则直接返回错误响应
  //    if (uri_state_ != URIState::PARSE_URI_ERROR) {
  //      process_state_ = ProcessState::STATE_PARSE_VERSION;
  //      break;
  //    } else {
  //      process_state_ = ProcessState::STATE_ERROR;
  //      break;
  //    }
  //
  //    // TODO 解析headers
  //  } while (false);
  //  // 获得http响应
  //  if (process_state_ == ProcessState::STATE_ERROR)
  //    handle_error();
  //  else if (process_state_ == ProcessState::STATE_FINISH) {
  //    handle_reponse();
  //  }
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
