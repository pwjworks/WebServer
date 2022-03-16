#include "HttpData.h"
#include <gtest/gtest.h>

#define test_parse_request_line(str, expected) \
  do {                                         \
    http->setMInput(str);                      \
    EXPECT_EQ(http->parse_line(), expected);   \
  } while (0)

TEST(httpdata, parse_request_line) {
  auto http = std::make_shared<HttpData>();
  test_parse_request_line("GET / HTTP/1.1\r\n", LINE_STATUS::LINE_OK);
  test_parse_request_line("GET / HTTP/1.1\r", LINE_STATUS::LINE_OPEN);
  test_parse_request_line("GET / HTTP/1.1\r\\", LINE_STATUS::LINE_BAD);
  test_parse_request_line("GET / HTTP/1.1\\\n", LINE_STATUS::LINE_BAD);
  test_parse_request_line("GET / HTTP/1.1", LINE_STATUS::LINE_OPEN);
}