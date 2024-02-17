#include "tuple_utils.hpp"
#include "tuple_utils.hpp"

#include <tuple>
#include <type_traits>

#include "../simple_test.h"

TEST(apply, void_empty) {
  bool invoked = false;
  cls11::apply(
    [&invoked](auto... args) {
      EXPECT_EQ(0, sizeof...(args));
      invoked = true;
    },
    std::make_tuple());
  EXPECT_TRUE(invoked);
}

TEST(apply, void_single) {
  bool invoked = false;
  cls11::apply(
    [&invoked](auto... args) {
      EXPECT_EQ(1, sizeof...(args));
      invoked = true;
    },
    std::make_tuple(1));
  EXPECT_TRUE(invoked);
}

TEST(apply, void_forward_args) {
  [](auto){

    bool invoked = false;
    int x = 0, y = 0, z = 0, w = 0;
    std::tuple<int, const int, int&, const int&, int&&, const int&&> t{
        1, 2, x, y, std::move(z), std::move(w)};

    auto f = [](auto&&...) {};  // для тестирования компилируемости

    invoked = false;
    if constexpr(requires { cls11::apply(f, t); }) {
      cls11::apply(
        [&](auto&& a, auto&& b, auto&& c, auto&& d, auto&& e, auto&& f) {
          invoked = true;
          EXPECT_EQ(&a, &std::get<0>(t));
          EXPECT_EQ(&b, &std::get<1>(t));
          EXPECT_EQ(&c, &std::get<2>(t));
          EXPECT_EQ(&d, &std::get<3>(t));
          EXPECT_EQ(&e, &std::get<4>(t));
          EXPECT_EQ(&f, &std::get<5>(t));

          EXPECT_TRUE((std::is_same_v<int&,       decltype(a)>));
          EXPECT_TRUE((std::is_same_v<const int&, decltype(b)>));
          EXPECT_TRUE((std::is_same_v<int&,       decltype(c)>));
          EXPECT_TRUE((std::is_same_v<const int&, decltype(d)>));
          EXPECT_TRUE((std::is_same_v<int&,       decltype(e)>));
          EXPECT_TRUE((std::is_same_v<const int&, decltype(f)>));
        },
        t
      );
    }
    EXPECT_TRUE(invoked) << "не компилируется передача кортежа по &";

    invoked = false;
    if constexpr(requires { cls11::apply(f, std::as_const(t)); }) {
      cls11::apply(
        [&](auto&& a, auto&& b, auto&& c, auto&& d, auto&& e, auto&& f) {
          invoked = true;
          EXPECT_EQ(&a, &std::get<0>(t));
          EXPECT_EQ(&b, &std::get<1>(t));
          EXPECT_EQ(&c, &std::get<2>(t));
          EXPECT_EQ(&d, &std::get<3>(t));
          EXPECT_EQ(&e, &std::get<4>(t));
          EXPECT_EQ(&f, &std::get<5>(t));

          EXPECT_TRUE((std::is_same_v<const int&, decltype(a)>));
          EXPECT_TRUE((std::is_same_v<const int&, decltype(b)>));
          EXPECT_TRUE((std::is_same_v<int&,       decltype(c)>));
          EXPECT_TRUE((std::is_same_v<const int&, decltype(d)>));
          EXPECT_TRUE((std::is_same_v<int&,       decltype(e)>));
          EXPECT_TRUE((std::is_same_v<const int&, decltype(f)>));
        },
        std::as_const(t)
      );
    }
    EXPECT_TRUE(invoked) << "не компилируется передача кортежа по const&";

    invoked = false;
    if constexpr(requires { cls11::apply(f, std::move(t)); }) {
      cls11::apply(
        [&](auto&& a, auto&& b, auto&& c, auto&& d, auto&& e, auto&& f) {
          invoked = true;
          EXPECT_EQ(&a, &std::get<0>(t));
          EXPECT_EQ(&b, &std::get<1>(t));
          EXPECT_EQ(&c, &std::get<2>(t));
          EXPECT_EQ(&d, &std::get<3>(t));
          EXPECT_EQ(&e, &std::get<4>(t));
          EXPECT_EQ(&f, &std::get<5>(t));

          EXPECT_TRUE((std::is_same_v<int&&,       decltype(a)>));
          EXPECT_TRUE((std::is_same_v<const int&&, decltype(b)>));
          EXPECT_TRUE((std::is_same_v<int&,        decltype(c)>));
          EXPECT_TRUE((std::is_same_v<const int&,  decltype(d)>));
          EXPECT_TRUE((std::is_same_v<int&&,       decltype(e)>));
          EXPECT_TRUE((std::is_same_v<const int&&, decltype(f)>));
        },
        std::move(t)
      );
    }
    EXPECT_TRUE(invoked) << "не компилируется передача кортежа по &&";

    invoked = false;
    if constexpr(requires { cls11::apply(f, std::move(std::as_const(t))); }) {
      cls11::apply(
        [&](auto&& a, auto&& b, auto&& c, auto&& d, auto&& e, auto&& f) {
          invoked = true;
          EXPECT_EQ(&a, &std::get<0>(t));
          EXPECT_EQ(&b, &std::get<1>(t));
          EXPECT_EQ(&c, &std::get<2>(t));
          EXPECT_EQ(&d, &std::get<3>(t));
          EXPECT_EQ(&e, &std::get<4>(t));
          EXPECT_EQ(&f, &std::get<5>(t));

          EXPECT_TRUE((std::is_same_v<const int&&, decltype(a)>));
          EXPECT_TRUE((std::is_same_v<const int&&, decltype(b)>));
          EXPECT_TRUE((std::is_same_v<int&,        decltype(c)>));
          EXPECT_TRUE((std::is_same_v<const int&,  decltype(d)>));
          EXPECT_TRUE((std::is_same_v<int&&,       decltype(e)>));
          EXPECT_TRUE((std::is_same_v<const int&&, decltype(f)>));
        },
        std::move(std::as_const(t))
      );
    }
    EXPECT_TRUE(invoked) << "не компилируется передача кортежа по const&&";

  }(0);
}

TEST(apply, forward_function) {
  // цель этого теста - убедиться в том, что apply не делает ненужных копирований функции.
  // функция f некопируема из-за некопируемой (но перемещаемой) связанной переменной p.

  [](auto) {
    bool invoked = false;
    auto f = [&invoked, p = std::make_unique<int>(123)](bool alive, int line) {
      EXPECT_EQ(alive, (bool)p) << "в строке " << line;
      invoked = true;
    };
    auto t = std::tuple{true, 123};

    if constexpr(requires{ cls11::apply(f, t); }) {
      cls11::apply(f, std::tuple{true, __LINE__});
    }
    EXPECT_TRUE(invoked) << "не удалось передать функцию по &";

    if constexpr(requires{ cls11::apply(std::as_const(f), t); }) {
      cls11::apply(std::as_const(f), std::tuple{true, __LINE__});
    }
    EXPECT_TRUE(invoked) << "не удалось передать функцию по const &";

    if constexpr(requires{ cls11::apply(std::move(f), t); }) {
      cls11::apply(std::move(f), std::tuple{true, __LINE__});
      // move само по себе не вызывает конструктор перемещения
      cls11::apply(std::move(f), std::tuple{true, __LINE__});
    }
    EXPECT_TRUE(invoked) << "не удалось передать функцию по &&";
  }(0);
}

TEST(apply, forward_result) {
  [](auto) {
    auto t = std::tuple{1};

    auto f = [&](auto) { return 123; };
    auto g = [&](auto) { return std::make_unique<int>(123); };

    EXPECT_TRUE(( std::is_same_v<int, decltype(f(t))> ));
    EXPECT_EQ(123, f(t));

    EXPECT_TRUE(( std::is_same_v<std::unique_ptr<int>, decltype(g(t))> ));
    EXPECT_EQ(123, *g(t));

    struct X {};
    X x;
    auto r = [&](auto) -> X& { return x; };
    auto с = [&](auto) -> const X& { return x; };
    auto m = [&](auto) -> X&& { return std::move(x); };
    auto cm = [&](auto) -> const X&& { return std::move(std::as_const(x)); };

    EXPECT_TRUE(( std::is_same_v<X&, decltype(r(t))> ));
    EXPECT_EQ(&x, &r(t));

    EXPECT_TRUE(( std::is_same_v<const X&, decltype(с(t))> ));
    EXPECT_EQ(&x, &с(t));

    EXPECT_TRUE(( std::is_same_v<X&&, decltype(m(t))> ));
    auto&& mt = m(t);
    EXPECT_EQ(&x, &mt);

    EXPECT_TRUE(( std::is_same_v<const X&&, decltype(cm(t))> ));
    auto&& cmt = cm(t);
    EXPECT_EQ(&x, &cmt);
  }(0);
}

TEST(apply, practical_example) {
  // квадрат длины вектора
  auto squared = [](auto... xs) {
    return ((xs * xs) + ...);
  };

  EXPECT_EQ(0, cls11::apply(squared, std::tuple{0}));
  EXPECT_EQ(0., cls11::apply(squared, std::tuple{0.}));
  EXPECT_EQ(0, cls11::apply(squared, std::tuple{0, 0}));

  EXPECT_EQ(25, cls11::apply(squared, std::tuple{3, 4}));
  EXPECT_EQ(3, cls11::apply(squared, std::tuple{-1, 1., 1UL}));
}

TESTING_MAIN()
