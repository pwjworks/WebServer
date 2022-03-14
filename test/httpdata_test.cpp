#include "HttpData.h"
#include <gtest/gtest.h>
#include <memory>

TEST(httpdata, parse_basic) {
  auto http = std::make_shared<HttpData>();
  http->setInBuffer("GET / HTTP/1.1\r");
  http->parse();
}

#define test_parse_method(str, expected)       \
  do {                                         \
    http->setInBuffer(str);                    \
    EXPECT_EQ(http->parse_method(), expected); \
  } while (0)

TEST(httpdata, parse_method) {
  auto http = std::make_shared<HttpData>();
  test_parse_method("GET / HTTP/1.1\r", HttpMethod::METHOD_GET);
  test_parse_method("OOO / HTTP/1.1\r", HttpMethod::METHOD_UNIMPLEMENTED);
  test_parse_method("POST / HTTP/1.1\r", HttpMethod::METHOD_POST);
  test_parse_method(" POST / HTTP/1.1\r", HttpMethod::METHOD_UNIMPLEMENTED);
}
