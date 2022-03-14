#include "HttpData.h"

using namespace std;


URIState HttpData::parse_URI() {
  if (in_buffer_[read_pos_] == '/') {
    read_pos_++;
    return URIState::PARSE_URI_SUCCESS;
  } else
    return URIState::PARSE_URI_ERROR;
}

HttpMethod HttpData::parse_method() {
  switch (in_buffer_[read_pos_]) {
    case 'G':
      if (read_pos_ < size_ - 2 && in_buffer_[read_pos_ + 1] == 'E' && in_buffer_[read_pos_ + 2] == 'T') {
        read_pos_ += 3;
        return HttpMethod::METHOD_GET;
      }
    case 'P':
      if (read_pos_ < size_ - 3 && in_buffer_[read_pos_ + 1] == 'O' && in_buffer_[read_pos_ + 2] == 'S' && in_buffer_[read_pos_ + 3] == 'T') {
        read_pos_ += 4;
        return HttpMethod::METHOD_POST;
      }
    default:
      return METHOD_UNIMPLEMENTED;
  }
}

void HttpData::parse_whitespace() {
  while (read_pos_ < size_ && (in_buffer_[read_pos_] == ' ' || in_buffer_[read_pos_] == '\t' || in_buffer_[read_pos_] == '\n' || in_buffer_[read_pos_] == '\r'))
    read_pos_++;
}

void HttpData::parse() {

  do {
    // 解析请求方法
    parse_whitespace();
    method_ = parse_method();
    // 检查状态，出现错误则直接返回错误响应
    if (method_ != HttpMethod::METHOD_UNIMPLEMENTED) {
      process_state_ = ProcessState::STATE_PARSE_URI;
    } else {
      process_state_ = ProcessState::STATE_ERROR;
      break;
    }

    if (process_state_ != ProcessState::STATE_PARSE_URI) break;
    // 解析请求URI
    parse_whitespace();
    uri_state_ = parse_URI();
    // 检查状态，出现错误则直接返回错误响应
    if (uri_state_ != URIState::PARSE_URI_ERROR) {
      process_state_ = ProcessState::STATE_FINISH;
      break;
    } else {
      process_state_ = ProcessState::STATE_ERROR;
      break;
    }

    // TODO 解析headers
  } while (false);
  // 获得http响应
  if (process_state_ == ProcessState::STATE_ERROR)
    handle_error();
  else if (process_state_ == ProcessState::STATE_FINISH) {
    handle_reponse();
  }
}

void HttpData::reset() {
  read_pos_ = 0;
  size_ = 0;
  method_ = HttpMethod::METHOD_UNIMPLEMENTED;
  process_state_ = ProcessState::STATE_PARSE_METHOD;
}


void HttpData::setInBuffer(const string &inBuffer) {
  reset();
  in_buffer_ = inBuffer;
  size_ = in_buffer_.size();
}


void HttpData::handle_error() {
}

void HttpData::handle_reponse() {
  out_buffer_ = "HTTP/1.1 200 OK\r\nContent-type: text/plain\r\n\r\nHello World";
}

const string &HttpData::getOutBuffer() const {
  return out_buffer_;
}
