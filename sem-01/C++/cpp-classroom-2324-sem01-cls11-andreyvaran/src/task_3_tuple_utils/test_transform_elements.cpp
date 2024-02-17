#include "tuple_utils.hpp"
#include "tuple_utils.hpp"

#include <cassert>
#include <utility>
#include <memory>

#include "../simple_test.h"

TEST(transform_elements, empty) {
  auto f = [](auto...) {
    FAIL();
    return 123;
  };

  EXPECT_EQ(std::tuple{}, cls11::transformElements(std::tuple{}, f));
}

TEST(transform_elements, single_that_matches) {
  auto f = [](int i) {
    return i * 0.5;
  };

  EXPECT_EQ(std::tuple{61.5}, cls11::transformElements(std::tuple{123}, f));
}

TEST(transform_elements, single_that_matches_and_returns_void) {
  int invoked = 0;
  auto f = [&](int i) {
    EXPECT_EQ(123, i);
    ++invoked;
  };

  EXPECT_EQ(std::tuple{123}, cls11::transformElements(std::tuple{123}, f));
  EXPECT_EQ(1, invoked);
}

TEST(transform_elements, single_that_mismatches) {
  auto f = [](const char*) {
    FAIL();
    return 12345;
  };

  EXPECT_EQ(std::tuple{123}, cls11::transformElements(std::tuple{123}, f));
}

TEST(transform_elements, references) {
  int x = 0;
  double y = 0;
  auto f = [&](int& elem) -> double& {
    EXPECT_EQ(&x, &elem);
    return y;
  };

  // обратите внимание, что на входе функции - кортеж ссылок
  // и, поскольку функция возвращает ссылку, то и результат тоже должен быть кортежом ссылок
  auto t = cls11::transformElements(std::tie(x), f);
  ASSERT_TRUE((std::is_same_v<std::tuple<double&>, decltype(t)>));
  EXPECT_EQ(&y, &std::get<0>(t));
}

TEST(transform_elements, move_semantics) {
  auto f = [](std::unique_ptr<int> p) {
    return std::make_unique<double>(*p * 2);
  };

  // этот тест - на perfect forwarding как аргументов, так и результата.
  auto t = cls11::transformElements(std::make_tuple(std::make_unique<int>(123)), f);
  EXPECT_EQ(246., *std::get<0>(t));
}

TEST(transform_elements, polymorph) {
  struct Modify {
    const char* operator()(float v) const { return v > 0 ? "YES" : "NO"; }
    int operator()(int* p)          const { return (*p)++; }
    void operator()(char* p)        const { *p = '!'; }
  };

  int x = 123;
  char s[] = "?..";

  auto t = cls11::transformElements(
      std::make_tuple(x, &x, s, &std::as_const(x)),
      Modify{});
  ASSERT_TRUE((std::is_same_v<
      std::tuple<const char*, int, char*, const int*>,
      decltype(t)>));

  // int как float -> const char*
  EXPECT_STREQ("YES", std::get<0>(t));

  // int* -> int
  EXPECT_EQ(123, std::get<1>(t));
  EXPECT_EQ(124, x);

  // char* -> void -> выполняем и возвращаем тот же char*
  EXPECT_EQ(s, std::get<2>(t));  // тот же адрес
  EXPECT_STREQ("!..", s);

  // const int* -> нет совпадений -> возвращаем тот же const int*
  EXPECT_EQ(&x, std::get<3>(t));
}

TESTING_MAIN()
