#include "any_all.hpp"
#include "any_all.hpp"

#include <optional>
#include <utility>

#include "../simple_test.h"

// конъюнкция и дизъюнкция обобщается до 0- и 1-элементных наборов следующим образом:
// мы мысленно добавляем в набор нейтральный элемент, который не влияет на результат
//
// x && true = x
// x || false = x
//
// поэтому
// all(x) == all(x, true) = x
// any(x) == any(x, false) = x
//
// all() == all(true) = true
// any() == any(false) = false


TEST(any, empty) {
  EXPECT_FALSE(cls11::any());
}

TEST(all, empty) {
  EXPECT_TRUE(cls11::all());
}

TEST(any, single) {
  auto unique_null = std::unique_ptr<int>{};
  auto unique_some = std::make_unique<int>(0);

  // type check
  EXPECT_EQ(typeid(bool), typeid(decltype(cls11::any(true))));
  EXPECT_EQ(typeid(bool), typeid(decltype(cls11::any(42))));
  EXPECT_EQ(typeid(bool), typeid(decltype(cls11::any(0))));
  EXPECT_EQ(typeid(bool), typeid(decltype(cls11::any(nullptr))));
  EXPECT_EQ(typeid(bool), typeid(decltype(cls11::any("nullptr"))));
  EXPECT_EQ(typeid(bool), typeid(decltype(cls11::any(std::optional<int>()))));
  EXPECT_EQ(typeid(bool), typeid(decltype(cls11::any(unique_null))));

  // true cases
  EXPECT_TRUE(cls11::any(true));
  EXPECT_TRUE(cls11::any(42));
  EXPECT_TRUE(cls11::any(""));
  EXPECT_TRUE(cls11::any(std::optional<int>{0}));
  EXPECT_TRUE(cls11::any(unique_some));

  // false cases
  EXPECT_FALSE(cls11::any(false));
  EXPECT_FALSE(cls11::any(0));
  EXPECT_FALSE(cls11::any(nullptr));
  EXPECT_FALSE(cls11::any(std::optional<int>{}));
  EXPECT_FALSE(cls11::any(unique_null));
}

TEST(all, single) {
  auto unique_null = std::unique_ptr<int>{};
  auto unique_some = std::make_unique<int>(0);

  // type check
  EXPECT_EQ(typeid(bool), typeid(decltype(cls11::all(true))));
  EXPECT_EQ(typeid(bool), typeid(decltype(cls11::all(42))));
  EXPECT_EQ(typeid(bool), typeid(decltype(cls11::all(0))));
  EXPECT_EQ(typeid(bool), typeid(decltype(cls11::all(nullptr))));
  EXPECT_EQ(typeid(bool), typeid(decltype(cls11::all("nullptr"))));
  EXPECT_EQ(typeid(bool), typeid(decltype(cls11::all(std::optional<int>()))));
  EXPECT_EQ(typeid(bool), typeid(decltype(cls11::all(unique_null))));

  // true cases
  EXPECT_TRUE(cls11::all(true));
  EXPECT_TRUE(cls11::all(42));
  EXPECT_TRUE(cls11::all(""));
  EXPECT_TRUE(cls11::all(std::optional<int>{0}));
  EXPECT_TRUE(cls11::all(unique_some));

  // false cases
  EXPECT_FALSE(cls11::all(false));
  EXPECT_FALSE(cls11::all(0));
  EXPECT_FALSE(cls11::all(nullptr));
  EXPECT_FALSE(cls11::all(std::optional<int>{}));
  EXPECT_FALSE(cls11::all(std::unique_ptr<int>{}));
}

TEST(any, mixed) {
  auto unique_null = std::unique_ptr<int>{};
  auto unique_some = std::make_unique<int>(0);

  // все false-подобны
  EXPECT_FALSE(cls11::any(0, nullptr, std::optional<int>{}, false, unique_null));

  // один true-подобный
  EXPECT_TRUE(cls11::any(42, nullptr, std::optional<int>{}, false, unique_null));
  EXPECT_TRUE(cls11::any(0, nullptr, std::optional<int>{0}, false, unique_null));
  EXPECT_TRUE(cls11::any(0, nullptr, std::optional<int>{}, false, unique_some));

  // все true-подобны
  EXPECT_TRUE(cls11::any(42, "", std::optional<int>{0}, true, unique_some));
}

TEST(all, mixed) {
  auto unique_null = std::unique_ptr<int>{};
  auto unique_some = std::make_unique<int>(0);

  // все false-подобны
  EXPECT_FALSE(cls11::all(0, nullptr, std::optional<int>{}, false, unique_null));

  // один false-подобный
  EXPECT_FALSE(cls11::all(0, "", std::optional<int>{0}, true, unique_some));
  EXPECT_FALSE(cls11::all(42, "", std::optional<int>{}, true, unique_some));
  EXPECT_FALSE(cls11::all(42, "", std::optional<int>{0}, true, unique_null));

  // все true-подобны
  EXPECT_TRUE(cls11::all(42, "", std::optional<int>{0}, true, unique_some));
}

TESTING_MAIN()
