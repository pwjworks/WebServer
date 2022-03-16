#include "HttpData.h"
#include <gtest/gtest.h>

#define test_parse_line(str, expected)       \
  do {                                       \
    http->setMInput(str);                    \
    EXPECT_EQ(http->parse_line(), expected); \
  } while (0)

TEST(httpdata, parse_line) {
  auto http = std::make_shared<HttpData>();
  test_parse_line("GET / HTTP/1.1\r\n", LINE_STATUS::LINE_OK);
  test_parse_line("GET / HTTP/1.1\r", LINE_STATUS::LINE_OPEN);
  test_parse_line("GET / HTTP/1.1\r\\", LINE_STATUS::LINE_BAD);
  test_parse_line("GET / HTTP/1.1\\\n", LINE_STATUS::LINE_BAD);
  test_parse_line("GET / HTTP/1.1", LINE_STATUS::LINE_OPEN);
}

#define test_parse_request_line(str, expected)          \
  do {                                                  \
    EXPECT_EQ(http->parse_request_line(str), expected); \
  } while (0)

TEST(httpdata, parse_request_line) {
  auto http = std::make_shared<HttpData>();
  test_parse_request_line("GET / HTTP/1.1\r\n", HTTP_CODE::GET_REQUEST);
  test_parse_request_line("GET http:// HTTP/1.1\r\n", HTTP_CODE::GET_REQUEST);
  test_parse_request_line("POST / HTTP/1.1\r\n", HTTP_CODE::GET_REQUEST);
  test_parse_request_line("GET / HTTP/1.2\r\n", HTTP_CODE::BAD_REQUEST);
  test_parse_request_line("GET / HTTP/1.1\r", HTTP_CODE::BAD_REQUEST);
  test_parse_request_line("GET https:// HTTP/1.1\r", HTTP_CODE::BAD_REQUEST);
}