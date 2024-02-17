#include "for_each.hpp"
#include "for_each.hpp"

#include <tuple>
#include <string>

#include "../simple_test.h"

TEST(for_each, empty) {
  cls11::for_each(std::tuple<>{}, [](auto){
    FAIL();
  });
}

TEST(for_each, single) {
  int visited = 0;
  cls11::for_each(std::tuple{123}, [&](auto i) { visited = i; });
  EXPECT_EQ(123, visited);
}

TEST(for_each, noncopyable) {
  // хак с условной компиляцией
  [](auto) {
    struct Noncopyable {
      Noncopyable() = default;
      Noncopyable(Noncopyable&) = delete;
      Noncopyable& operator=(Noncopyable&) = delete;

      Noncopyable(int, int, int) {}
    };

    std::tuple<Noncopyable> t;
    auto f = [&](const Noncopyable& elem) {
      EXPECT_EQ(&std::get<0>(t), &elem);
    };

    if constexpr (requires { cls11::for_each(t, f); }) {
      cls11::for_each(t, f);
    } else {
      FAIL() << "нужно реализовать perfect forwarding!";
    }
  }(0);
}

TEST(for_each, proper_order) {
  int result = 0;

  struct Visit {
    int& result;

    void operator()(int elem) const {
      result = result * 10 + elem;
    }
  };

  cls11::for_each(std::tuple{1, 2, 3, 4}, Visit{result});
  EXPECT_EQ(1234, result);
}

TEST(for_each, proper_order_of_stateful_function) {
  int result = 0;

  struct Visit {
    int& result;
    int seed;

    void operator()(int elem) {
      result = result * 10 + elem + seed;
      seed++;
    }
  };

  cls11::for_each(std::tuple{1, 2, 3, 4}, Visit{result, 0});
  EXPECT_EQ(1357, result);
}

#if 1
TEST(for_each, modify_elements) {
  // хак с условной компиляцией
  [](auto) {
    struct Modify {
      void operator()(int& i) const { i++; }
      void operator()(double& d) const { d *= 10; }
      void operator()(std::string& s) const { s += "!"; }

      // это ловушки!
      void operator()(const int& i) const { FAIL(); }
      void operator()(const double& d) const { FAIL(); }
      void operator()(const std::string& s) const { FAIL(); }
    };

    std::tuple<int, double, std::string, int> t {123, 123.0, "hello", 0};

    if constexpr (requires { cls11::for_each(t, Modify{}); }) {
      cls11::for_each(t, Modify{});
      EXPECT_EQ(124, std::get<0>(t));
      EXPECT_EQ(1230.0, std::get<1>(t));
      EXPECT_EQ("hello!", std::get<2>(t));
      EXPECT_EQ(1, std::get<3>(t));
    } else {
      FAIL() << "не компилируется передача кортежа по ссылке";
    }
  }(0);
}
#endif
TEST(for_each, modify_elements_of_tie) {
  struct Modify {
    void operator()(int& i) const { i++; }
    void operator()(double& d) const { d *= 10; }
    void operator()(std::string& s) const { s += "!"; }
  };

  int x = 123;
  double y = 123.0;
  std::string z = "hello";

  cls11::for_each(std::tie(x, y, z, z, z), Modify{});
  EXPECT_EQ(124, x);
  EXPECT_EQ(1230.0, y);
  EXPECT_EQ("hello!!!", z);
}

TEST(for_each, modify_elements_of_mixed_refs) {
  // хак с условной компиляцией
  [](auto) {
    enum RefType {
      REF = 1,
      CONST_REF = 2,
      RVALUE_REF = 3,
      CONST_RVALUE_REF = 4,
    };

    // все переменные у нас - честные не-константы,
    // поэтому хак со снятием константности тут законный
    // (мы ведь не копируем кортеж в for_each, правда же?)
    struct Modify {
      void operator()(      int&  i) const { const_cast<int&>(i) = REF;              }
      void operator()(const int&  i) const { const_cast<int&>(i) = CONST_REF;        }
      void operator()(      int&& i) const { const_cast<int&>(i) = RVALUE_REF;       }
      void operator()(const int&& i) const { const_cast<int&>(i) = CONST_RVALUE_REF; }
    };

    struct XYZ { int x = 0, y = 0, z = 0; };
    using T = std::tuple<int, const int, int&, int&&, const int&>;

    // поведение for_each в точности совпадает с семантикой
    // ручного поэлементного вызова функции для каждого элемента

    // неконстантный кортеж со значениями и ссылками
    // std::get превращает rvalue-reference в обычную ссылку
    if constexpr (requires(T t) { cls11::for_each(t, Modify{}); }) {
      XYZ xyz;
      T t {0, 0, xyz.x, std::move(xyz.y), std::as_const(xyz.z)};

      // эталон
      XYZ abc;
      T e {0, 0, abc.x, std::move(abc.y), std::as_const(abc.z)};

      cls11::for_each(t, Modify{});

      Modify{}(std::get<0>(e));
      Modify{}(std::get<1>(e));
      Modify{}(std::get<2>(e));
      Modify{}(std::get<3>(e));
      Modify{}(std::get<4>(e));

      ASSERT_EQ(std::get<0>(e), REF);  // int -> int&
      ASSERT_EQ(std::get<1>(e), CONST_REF);  // const int -> const int&
      ASSERT_EQ(abc.x, REF);  // int&
      ASSERT_EQ(abc.y, REF);  // int&& -> int&
      ASSERT_EQ(abc.z, CONST_REF);  // const int&

      EXPECT_EQ(std::get<0>(e), std::get<0>(t));
      EXPECT_EQ(std::get<1>(e), std::get<1>(t));
      EXPECT_EQ(abc.x, xyz.x);
      EXPECT_EQ(abc.y, xyz.y);
      EXPECT_EQ(abc.z, xyz.z);
    } else {
      ADD_FAILURE() << "не компилируется передача кортежа по &";
    }

    // константный кортеж
    // накладывает константность на свои значения
    // но не накладывает константность на ссылки!
    if constexpr (requires(T t) { cls11::for_each(std::as_const(t), Modify{}); }) {
      XYZ xyz;
      T t {0, 0, xyz.x, std::move(xyz.y), std::as_const(xyz.z)};

      // эталон
      XYZ abc;
      T e {0, 0, abc.x, std::move(abc.y), std::as_const(abc.z)};

      cls11::for_each(std::as_const(t), Modify{});

      Modify{}(std::get<0>(std::as_const(e)));
      Modify{}(std::get<1>(std::as_const(e)));
      Modify{}(std::get<2>(std::as_const(e)));
      Modify{}(std::get<3>(std::as_const(e)));
      Modify{}(std::get<4>(std::as_const(e)));

      ASSERT_EQ(std::get<0>(e), CONST_REF);  // int + const -> int&
      ASSERT_EQ(std::get<1>(e), CONST_REF);  // const int + const -> const int&
      ASSERT_EQ(abc.x, REF);  // int&
      ASSERT_EQ(abc.y, REF);  // int&& -> int&
      ASSERT_EQ(abc.z, CONST_REF);  // const int&

      EXPECT_EQ(std::get<0>(e), std::get<0>(t));
      EXPECT_EQ(std::get<1>(e), std::get<1>(t));
      EXPECT_EQ(abc.x, xyz.x);
      EXPECT_EQ(abc.y, xyz.y);
      EXPECT_EQ(abc.z, xyz.z);
    } else {
      ADD_FAILURE() << "не компилируется передача кортежа по const &";
    }

    // rvalue-reference на кортеж
    // передаёт свои значения по rvalue-reference
    // (естественно, не снимая константности с константных значений)
    // std::get в этом случае сохраняет rvalue-reference
    // но не влияет на настоящую неконстантную ссылку
    if constexpr (requires(T t) { cls11::for_each(std::move(t), Modify{}); }) {
      XYZ xyz;
      T t {0, 0, xyz.x, std::move(xyz.y), std::as_const(xyz.z)};

      // эталон
      XYZ abc;
      T e {0, 0, abc.x, std::move(abc.y), std::as_const(abc.z)};

      cls11::for_each(std::move(t), Modify{});

      Modify{}(std::get<0>(std::move(e)));
      Modify{}(std::get<1>(std::move(e)));
      Modify{}(std::get<2>(std::move(e)));
      Modify{}(std::get<3>(std::move(e)));
      Modify{}(std::get<4>(std::move(e)));

      ASSERT_EQ(std::get<0>(e), RVALUE_REF);  // int + && -> int&&
      ASSERT_EQ(std::get<1>(e), CONST_RVALUE_REF);  // const int + && -> const int&
      ASSERT_EQ(abc.x, REF);  //
      ASSERT_EQ(abc.y, RVALUE_REF);
      ASSERT_EQ(abc.z, CONST_REF);

      EXPECT_EQ(std::get<0>(e), std::get<0>(t));
      EXPECT_EQ(std::get<1>(e), std::get<1>(t));
      EXPECT_EQ(abc.x, xyz.x);
      EXPECT_EQ(abc.y, xyz.y);
      EXPECT_EQ(abc.z, xyz.z);
    } else {
      ADD_FAILURE() << "не компилируется передача кортежа по &&";
    }

    // const rvalue-reference (да, такое тоже возможно!) на кортеж
    // передаёт свои значения по const rvalue-reference
    // ссылки остаются ссылками, а rvalue-ссылки - rvalue-ссылками
    if constexpr (requires(T t) { cls11::for_each(std::move(std::as_const(t)), Modify{}); }) {
      XYZ xyz;
      T t {0, 0, xyz.x, std::move(xyz.y), std::as_const(xyz.z)};

      // эталон
      XYZ abc;
      T e {0, 0, abc.x, std::move(abc.y), std::as_const(abc.z)};

      cls11::for_each(std::move(std::as_const(t)), Modify{});

      Modify{}(std::get<0>(std::move(std::as_const(e))));
      Modify{}(std::get<1>(std::move(std::as_const(e))));
      Modify{}(std::get<2>(std::move(std::as_const(e))));
      Modify{}(std::get<3>(std::move(std::as_const(e))));
      Modify{}(std::get<4>(std::move(std::as_const(e))));

      ASSERT_EQ(std::get<0>(e), CONST_RVALUE_REF);  // int + const && -> int&&
      ASSERT_EQ(std::get<1>(e), CONST_RVALUE_REF);  // const int + const && -> const int&
      ASSERT_EQ(abc.x, REF);
      ASSERT_EQ(abc.y, RVALUE_REF);
      ASSERT_EQ(abc.z, CONST_REF);

      EXPECT_EQ(std::get<0>(e), std::get<0>(t));
      EXPECT_EQ(std::get<1>(e), std::get<1>(t));
      EXPECT_EQ(abc.x, xyz.x);
      EXPECT_EQ(abc.y, xyz.y);
      EXPECT_EQ(abc.z, xyz.z);
    } else {
      ADD_FAILURE() << "не компилируется передача кортежа по const &&";
    }
  }(0);
}

TESTING_MAIN()
