#include "tuple_utils.hpp"
#include "tuple_utils.hpp"

#include <cassert>
#include <map>
#include <string>
#include <unordered_map>

#include "../simple_test.h"

TEST(at_many, empty) {
  [](auto){
    if constexpr (requires { cls11::at_many("hello"); }) {
      // реализовывать это необязательно
      // но если сделаете, то было бы логично ожидать такое поведение:
      // ничего на входе - успешное ничего на выходе
      EXPECT_EQ(std::tuple{}, cls11::at_many("hello" /* с пустым набором контейнеров */));
    } else {
      // не реализовано, ну и ладно
    }
  }(0);
}

TEST(at_many, single) {
  const std::map<std::string, int> m1 = {
    {"hello", 1},
    {"world", 2}};
  auto t = cls11::at_many("hello", m1);
  EXPECT_EQ(std::tuple{1}, t);
}

TEST(at_many, multiple) {
  const std::map<std::string, int> m1 = {
    {"hello", 1},
    {"world", 2}};
  const std::unordered_map<std::string, std::string> m2 = {
    {"hello", "привет"},
    {"goodbye", "пока"}};
  auto t = cls11::at_many("hello", m1, m2, m1);
  EXPECT_EQ(std::tuple(1, "привет", 1), t);
}

TEST(at_many, failure) {
  const std::map<std::string, int> m1 = {
    {"hello", 1},
    {"world", 2}};
  const std::unordered_map<std::string, std::string> m2 = {
    {"hello", "привет"},
    {"goodbye", "пока"}};
  const std::map<std::string, double> m3 = {
    {"aaa", 3},
    {"bbb", 4}};
  EXPECT_THROW(cls11::at_many("hello", m1, m2, m3), std::out_of_range);
}

TESTING_MAIN()
