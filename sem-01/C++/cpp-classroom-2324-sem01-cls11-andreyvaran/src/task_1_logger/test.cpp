#include "logger.hpp"
#include "logger.hpp"

#include <memory>
#include <string>
#include <iostream>
#include <sstream>

#include "../simple_test.h"

class CoutGuard {
  std::ostringstream oss;
  std::streambuf* old_buffer;

  CoutGuard(CoutGuard&) = delete;
  CoutGuard& operator=(CoutGuard&) = delete;
public:
  CoutGuard() : old_buffer(std::cout.rdbuf()) {
    std::cout.rdbuf(oss.rdbuf());
  }
  ~CoutGuard() {
    std::cout.rdbuf(old_buffer);
  }
  std::string str() const { return oss.str(); }
};

TEST(logger, void_without_args) {
  CoutGuard cout_guard;

  auto f = cls11::logging_wrapper(
    []() { std::cout << "(inside)"; },
    "(begin)",
    "(end)"
  );
  EXPECT_EQ("", cout_guard.str());

  f();
  EXPECT_EQ("(begin)(inside)(end)", cout_guard.str());

  f();
  EXPECT_EQ("(begin)(inside)(end)(begin)(inside)(end)", cout_guard.str());
}

TEST(logger, void_with_args) {
  CoutGuard cout_guard;

  auto f = cls11::logging_wrapper(
    [](int x, int& y) {
      std::cout << "[" << x << "," << (++y) << "]";
    },
    "(begin)",
    "(end)"
  );
  EXPECT_EQ("", cout_guard.str());

  int y = 2;

  f(1, y);
  EXPECT_EQ("(begin)[1,3](end)", cout_guard.str());
  EXPECT_EQ(3, y);

  f(2, y);
  EXPECT_EQ("(begin)[1,3](end)(begin)[2,4](end)", cout_guard.str());
  EXPECT_EQ(4, y);
}

TEST(logger, result_without_args) {
  CoutGuard cout_guard;

  int x = 0;
  auto f = cls11::logging_wrapper(
    [&x]() {
      std::cout << "[" << (++x) << "]";
      return x * 10;
    },
    "(begin)",
    "(end)"
  );
  EXPECT_EQ("", cout_guard.str());

  EXPECT_EQ(10, f());
  EXPECT_EQ("(begin)[1](end)", cout_guard.str());

  EXPECT_EQ(20, f());
  EXPECT_EQ("(begin)[1](end)(begin)[2](end)", cout_guard.str());
}

TEST(logger, return_with_args) {
  CoutGuard cout_guard;

  auto f = cls11::logging_wrapper(
    [](int x, int& y) {
      std::cout << "[" << x << "," << (++y) << "]";
      return x * 100 + y;
    },
    "(begin)",
    "(end)"
  );
  EXPECT_EQ("", cout_guard.str());

  int y = 2;

  EXPECT_EQ(103, f(1, y));
  EXPECT_EQ("(begin)[1,3](end)", cout_guard.str());
  EXPECT_EQ(3, y);

  EXPECT_EQ(204, f(2, y));
  EXPECT_EQ("(begin)[1,3](end)(begin)[2,4](end)", cout_guard.str());
  EXPECT_EQ(4, y);
}

TEST(logger, perfect_forwarding_function) {
  CoutGuard cout_guard;

  auto f = cls11::logging_wrapper(
    [p = std::make_unique<int>(100)]() { std::cout << ++*p; },
    "{", "}");

  f();
  EXPECT_EQ("{101}", cout_guard.str());

  f();
  EXPECT_EQ("{101}{102}", cout_guard.str());
}

TEST(logger, perfect_forwarding_argument) {
  CoutGuard cout_guard;

  auto f = cls11::logging_wrapper(
    [](std::unique_ptr<int> p) { std::cout << ++*p; },
    "{", "}");

  auto p = std::make_unique<int>(100);
  f(std::move(p));
  EXPECT_EQ(nullptr, p.get());
  EXPECT_EQ("{101}", cout_guard.str());
}

TEST(logger, perfect_forwarding_result) {
  CoutGuard cout_guard;

  auto f = cls11::logging_wrapper(
    []() { std::cout << "ok"; return std::make_unique<int>(100); },
    "{", "}");

  auto p = f();
  EXPECT_EQ("{ok}", cout_guard.str());
  ASSERT_NE(nullptr, p.get());
  EXPECT_EQ(100, *p);
}

TEST(logger, polymorph) {
  CoutGuard cout_guard;

  auto f = cls11::logging_wrapper(
    [](auto... xs) {
      if constexpr (sizeof...(xs) == 1) {
        return 0;
      } else {
        return;
      }
    },
    "{", "}");

  EXPECT_TRUE((std::is_same_v<void, decltype(f())>));
  EXPECT_TRUE((std::is_same_v<int, decltype(f(1))>));
  EXPECT_TRUE((std::is_same_v<void, decltype(f(1, 2))>));
}

TESTING_MAIN()
