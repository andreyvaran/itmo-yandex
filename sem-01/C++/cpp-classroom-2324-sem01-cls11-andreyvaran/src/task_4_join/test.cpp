#include "join.hpp"
#include <string>
#include <utility>

#include "../simple_test.h"

// #define ACCEPT_FREE_FUNCTIONS
// #define ALLOW_SAME_FUNCTOR_CLASSES


TEST(join, single_return_void) {
  int invoked = 0;
  auto fun = cls11::join(
    [&](auto x) {
      EXPECT_EQ(123, x);
      ++invoked;
    }
  );

  EXPECT_TRUE((std::is_same_v<void, decltype(fun(123))>));
  EXPECT_TRUE((std::is_same_v<void, decltype(fun(123.0))>));

  fun(123);
  EXPECT_EQ(1, invoked);
  fun(123.0);
  EXPECT_EQ(2, invoked);
}

TEST(join, single_return_value) {
  auto fun = cls11::join(
    [&](auto* x) {
      ASSERT_NE(nullptr, x);
      return *x;
    }
  );

  int i = 12345;
  double j = 1234.5;
  const char* s = "S";

  EXPECT_TRUE((std::is_same_v<int, decltype(fun(&i))>));
  EXPECT_TRUE((std::is_same_v<double, decltype(fun(&j))>));
  EXPECT_TRUE((std::is_same_v<char, decltype(fun(s))>));

  EXPECT_EQ(i, fun(&i));
  EXPECT_EQ(j, fun(&j));
  EXPECT_EQ(*s, fun(s));
}

TEST(join, single_return_reference) {
  auto fun = cls11::join(
    [&](auto* x) -> auto& {
      ASSERT_NE(nullptr, x);
      return *x;
    }
  );

  int i = 12345;

  EXPECT_TRUE((std::is_same_v<int&, decltype(fun(&i))>));

  EXPECT_EQ(&i, &fun(&i));
}

TEST(join, single_perfect_forward_arg) {
  auto fun = cls11::join(
    [&](std::unique_ptr<int> p) {
      return *p;
    }
  );

  auto i = std::make_unique<int>(123);
  EXPECT_TRUE((std::is_same_v<int, decltype(fun(std::move(i)))>));

  EXPECT_EQ(123, fun(std::move(i)));
  EXPECT_EQ(nullptr, i.get());
}

TEST(join, single_perfect_forward_return) {
  auto fun = cls11::join(
    [&](int* p) -> int&& {
      return std::move(*p);
    }
  );

  int i = 123;
  EXPECT_TRUE((std::is_same_v<int&&, decltype(fun(&i))>));

  int&& j = fun(&i);
  EXPECT_EQ(&i, &j);
}

TEST(join, perfect_forward_function) {
  auto fun = cls11::join(
    [p = std::make_unique<int>(12300)](int i) {
      ASSERT_NE(nullptr, p);
      return *p + i;
    }
  );

  EXPECT_TRUE((std::is_same_v<int, decltype(fun(45))>));
  EXPECT_EQ(12345, fun(45));
  EXPECT_EQ(12367, fun(67));
}

TEST(join, two_functions_with_different_return) {
  auto fun = cls11::join(
    [](int x) -> std::string { return std::to_string(x); },
    [](int* p) -> int { return *p; }
  );

  int x = 123;
  EXPECT_TRUE((std::is_same_v<std::string, decltype(fun(x))>));
  EXPECT_TRUE((std::is_same_v<int, decltype(fun(&x))>));

  EXPECT_EQ("123", fun(x));
  EXPECT_EQ(123, fun(&x));
}

TEST(join, two_functions_with_same_return) {
  auto fun = cls11::join(
      [](std::string const& s) { return "it's a string"; },
      [](std::pair<int, int> const& p) { return "it's a pair"; }
  );

  EXPECT_STREQ("it's a string", fun("string"));
  EXPECT_STREQ("it's a pair", fun(std::pair(1, 2)));
}

#ifdef ACCEPT_FREE_FUNCTIONS
TEST(join, accept_free_functions) {
  struct Funs {
    static int foo(int x) { return x; }
    static void bar(const char* s) {}
  };

  auto fun = cls11::join(&Funs::foo, &Funs::bar);
  EXPECT_TRUE((std::is_same_v<int, decltype(fun(123))>));
  EXPECT_TRUE((std::is_same_v<void, decltype(fun("123"))>));

  EXPECT_EQ(123, fun(123));
  fun("123");
}
#endif  // ACCEPT_FREE_FUNCTIONS

TEST(join, many_functions) {
  auto fun = cls11::join(
      [](int*    p) { return *p; },
      [](short*  p) { return *p; },
      [](long*   p) { return *p; },
      [](double* p) { return *p; },
      [](char*   p) { return *p; }
  );

  int x = 1;
  short y = 2;
  long z = 3;
  double t = 4.5;
  char u = '6';

  EXPECT_EQ(x, fun(&x));
  EXPECT_EQ(y, fun(&y));
  EXPECT_EQ(z, fun(&z));
  EXPECT_EQ(t, fun(&t));
  EXPECT_EQ(u, fun(&u));
}

TEST(join, sfinae_if_no_match) {
  [](auto) {
    auto f = [](int* p) {};
    auto g = [](long* p) -> int { return 0; };
    auto h = [](char* p) {};
    auto fun = cls11::join(f, g, h);

    long x = 0;
    // доказательство того, что напрямую SFINAE работает:
    // не удалось подставить int, но это не ошибка компиляции
    ASSERT_FALSE((requires { f(123); }));
    ASSERT_FALSE((requires { g(123); }));
    ASSERT_FALSE((requires { h(123); }));

    ASSERT_TRUE ((requires { g(&x); }));
    ASSERT_TRUE ((std::is_same_v<int, decltype(g(&x))>));

    // проверяем, что SFINAE так же работает и с join
    EXPECT_FALSE((requires { fun(123); }));

    EXPECT_TRUE ((requires { fun(&x); }));
    EXPECT_TRUE ((std::is_same_v<int, decltype(fun(&x))>));
  }(0);
}

TEST(join, cascaded_join_v1) {
  auto fun = cls11::join(
      cls11::join(
          [](int*    p) { return *p; },
          [](short*  p) { return *p; },
          cls11::join(
              [](long*   p) { return *p; },
              [](double* p) { return *p; },
              [](char*   p) { return *p; }
          )
      )
  );

  int x = 1;
  short y = 2;
  long z = 3;
  double t = 4.5;
  char u = '6';

  EXPECT_EQ(x, fun(&x));
  EXPECT_EQ(y, fun(&y));
  EXPECT_EQ(z, fun(&z));
  EXPECT_EQ(t, fun(&t));
  EXPECT_EQ(u, fun(&u));
}

TEST(join, cascaded_join_v2) {
  auto fun = cls11::join(
      cls11::join(
          [](int*    p) { return *p; },
          [](short*  p) { return *p; }
      ),
      cls11::join(
          [](long*   p) { return *p; },
          [](double* p) { return *p; },
          [](char*   p) { return *p; }
      )
  );

  int x = 1;
  short y = 2;
  long z = 3;
  double t = 4.5;
  char u = '6';

  EXPECT_EQ(x, fun(&x));
  EXPECT_EQ(y, fun(&y));
  EXPECT_EQ(z, fun(&z));
  EXPECT_EQ(t, fun(&t));
  EXPECT_EQ(u, fun(&u));
}

TEST(join, best_appropriate) {

  auto fun = cls11::join(
      [](int*) { return 0; },
      [](int) { return 1; },
      [](bool) { return 2; },

      // дубликаты могут быть только среди неиспользуемых
      // (иначе это вызовет ошибку неоднозначности)
#ifdef ALLOW_SAME_FUNCTOR_CLASSES
      std::function<int(double*)>{[](auto) { return 3; }},
      std::function<int(double*)>{[](auto) { return 4; }},
#else
      [](double*) { return 3; },
      [](double*) { return 4; },
#endif

      [](...) { return 5; }  // ловушка только для тривиальных типов!
  );

  int x = 0;
  long y = 0;

  auto px = &x;
  auto py = &y;

  struct E {} e;

  EXPECT_EQ(0, fun(px));
  EXPECT_EQ(1, fun(x));
  EXPECT_EQ(2, fun(py));
  EXPECT_EQ(5, fun(e));
}

TEST(join, polymorph_functions_overlap_signatures) {
  struct F {
    int operator()(int*) { return 0; }  // не используется
    int operator()(int) { return 1; }
  };
  struct G {
    int operator()(int*) { return 2; }  // не используется
    int operator()(bool) { return 3; }
  };

  auto fun = cls11::join(F{}, G{});

  EXPECT_EQ(1, fun(1));
  EXPECT_EQ(3, fun(true));
}


TESTING_MAIN()
