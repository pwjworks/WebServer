#include "HttpData.h"
#include <gtest/gtest.h>

#define test_parse_line(str, expected)       \
  do {                                       \
    char s[] = str;                          \
    http->setMInput(s);                      \
    EXPECT_EQ(http->parse_line(), expected); \
  } while (0)

TEST(httpdata, parse_line) {
  auto http = std::make_shared<HttpData>();
  test_parse_line("GET / HTTP/1.1\r\n", LINE_STATUS::LINE_OK);
  test_parse_line("GET / HTTP/1.1\r", LINE_STATUS::LINE_OPEN);
  test_parse_line("GET / HTTP/1.1\r\\", LINE_STATUS::LINE_OPEN);
  test_parse_line("GET / HTTP/1.1\\\n", LINE_STATUS::LINE_BAD);
  test_parse_line("GET / HTTP/1.1", LINE_STATUS::LINE_OPEN);
}

#define test_parse_request_line(str, expected)           \
  do {                                                   \
    char test[] = str;                                   \
    EXPECT_EQ(http->parse_request_line(test), expected); \
  } while (0)

TEST(httpdata, parse_request_line) {
  auto http = std::make_shared<HttpData>();
  test_parse_request_line("GET / HTTP/1.1", HTTP_CODE::NO_REQUEST);
  test_parse_request_line("GET http://test/ HTTP/1.1", HTTP_CODE::NO_REQUEST);
  test_parse_request_line("POST / HTTP/1.1", HTTP_CODE::NO_REQUEST);
  test_parse_request_line("GET / HTTP/1.2", HTTP_CODE::BAD_REQUEST);
  test_parse_request_line("GET / HTTP/1.1\t", HTTP_CODE::BAD_REQUEST);
  test_parse_request_line("GET https:// HTTP/1.1\n", HTTP_CODE::BAD_REQUEST);
}

#define test_parse_headers(str, expected)           \
  do {                                              \
    char test[] = str;                              \
    EXPECT_EQ(http->parse_headers(test), expected); \
  } while (0)

TEST(httpdata, parse_headers) {
  auto http = std::make_shared<HttpData>();
  test_parse_headers("\0", HEADER_STAUS::HEADER_OK);
  test_parse_headers("Connection : keep-alive", HEADER_STAUS::HEADER_OK);
  test_parse_headers("Connection: keep-alive", HEADER_STAUS::HEADER_OK);
  test_parse_headers("Connection:keep-alive", HEADER_STAUS::HEADER_OK);
  test_parse_headers("Connection:", HEADER_STAUS::HEADER_BAD);
}