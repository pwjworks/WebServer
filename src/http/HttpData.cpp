#include "HttpData.h"
#include <iostream>

using namespace std;


void HttpData::parseURI() {
  size_t pos = in_buffer_.find('\r', read_pos_);
  string request_line = in_buffer_.substr(0, pos);
  cout << request_line << endl;
  //auto iter = request_line.begin();
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
  parse_whitespace();
  method_ = parse_method();
  if (method_ != HttpMethod::METHOD_UNIMPLEMENTED)
    processState_ = ProcessState::STATE_PARSE_URI;
  else
    handle_error();
}


void HttpData::setInBuffer(const string &inBuffer) {
  in_buffer_ = inBuffer;
  read_pos_ = 0;
  size_ = in_buffer_.size();
}

void HttpData::handle_error() {
}
