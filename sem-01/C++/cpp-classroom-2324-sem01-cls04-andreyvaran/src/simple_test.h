#pragma once

#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <exception>
#include <cstring>

// some tests interact with std::cout, so let's use separate stream
#define OUTPUT_STREAM() std::cerr

namespace simple_print {

static constexpr const char* bar = "-------------------";
static constexpr const char* barbar = "===================";
static constexpr const char* red = "\x1b[31m";
static constexpr const char* green = "\x1b[32m";
static constexpr const char* yellow = "\x1b[33m";
static constexpr const char* blue = "\x1b[34m";
static constexpr const char* normal = "\x1b[0m";

struct colored_cout_line {
  static bool is_colored() {
    static const bool tty = (isatty(fileno(stdout)));
    return tty;
  }

  explicit colored_cout_line(const char* color) {
    if (is_colored()) OUTPUT_STREAM() << color;
  }
  colored_cout_line(colored_cout_line const&) = delete;
  ~colored_cout_line() {
    if (is_colored()) OUTPUT_STREAM() << normal;
    OUTPUT_STREAM() << std::endl;
  }

  std::ostream& ost() const { return OUTPUT_STREAM(); }
  std::ostream& operator << (const auto& arg) { return ost() << arg; }
};

inline void verbose_print_string(std::ostream& ost, const char* s, size_t n) {
  auto flags = ost.flags();
  ost << "\"";
  for ( ; n; ++s, --n) {
    char c = *s;
    if (c == '\n') ost << "\\n";
    else if (c == '\r') ost << "\\r";
    else if (c == '\t') ost << "\\t";
    else if (c >= 0 && c < 32) ost << "\\x" << std::hex << std::setw(2) << std::setfill('0') << +c;
    else ost << c;
  }
  ost << "\"";
  ost.flags(flags);
}
inline void verbose_print_string(std::ostream& ost, const std::string& arg) {
  verbose_print_string(ost, arg.c_str(), arg.size());
}
inline void verbose_print_string(std::ostream& ost, const char* arg) {
  verbose_print_string(ost, arg, strlen(arg));
}

void verbose_print(std::ostream& ost, const auto& arg) {
  using T = std::decay_t<decltype((arg))>;
  if constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, const char*> || std::is_same_v<T, char*>) {
    verbose_print_string(ost, arg);
  } else if constexpr (std::is_same_v<T, bool>) {
    ost << std::boolalpha << arg;
  } else if constexpr (requires { ost << arg; }) {
    ost << arg;
  } else if constexpr (std::is_same_v<T, std::type_info>) {
    ost << "typeid name = " << arg.name();
  } else {
    ost << typeid(T).name() << " [" << sizeof(T) << " bytes] @ " << &arg;
  }
}

template<class T> struct verbose {
  const T& arg; verbose(const T& a) : arg(a) {}
  friend std::ostream& operator << (std::ostream& ost, const verbose& v) {
    verbose_print(ost, v.arg);
    return ost;
  }
};
template<class T> verbose(T) -> verbose<T>;

}  // namespace simple_print

namespace simple_test {

inline bool& show_green_assertions() {
  static bool flag = false;
  return flag;
}
inline bool show_green_assertions(bool flag) {
  bool old_flag = show_green_assertions();
  show_green_assertions() = flag;
  return old_flag;
}

struct assertion_fault {};  // out of std::exception hierarchy

struct TestCase {
  static TestCase*& first() { static TestCase* t = nullptr; return t; }
  static TestCase*& last() { static TestCase* t = nullptr; return t; }

  static TestCase*& current() { static TestCase* t = nullptr; return t; }

  // chain
  TestCase* m_next = nullptr;
  const char* m_suite;
  const char* m_name;
  void (*m_func)();
  bool m_enabled;

  // preset
  bool m_show_green_assertions = false;

  // result
  bool m_called = false;
  bool m_passed = false;

  static bool is_name_disabled(const char* name) {
    static const char kDisabled[] = "DISABLED";
    static const size_t nDisabled = strlen(kDisabled);
    return strncmp(name, kDisabled, nDisabled) == 0;
  }

  TestCase(const char* suite, const char* name, void(*func)(), bool enabled = true)
    : m_suite(suite)
    , m_name(name)
    , m_func(func)
    , m_enabled(enabled && !is_name_disabled(suite) && !is_name_disabled(name))
    , m_show_green_assertions(show_green_assertions())
  {
    if (first()) {
      last() = last()->m_next = this;
    } else {
      last() = first() = this;
    }
  }

  friend std::ostream& operator << (std::ostream& ost, TestCase const& t) {
    return ost << t.m_suite << "." << t.m_name;
  }

  static bool run_all() {
    int num_skipped = 0, num_passed = 0, num_failed = 0;
    for (TestCase* t = first(); t; t = t->m_next) {
      if (!t->m_enabled) {
        num_skipped++;
        continue;
      }

      current() = t;
      bool old_green_assertions = show_green_assertions(t->m_show_green_assertions);

      t->m_called = true;
      simple_print::colored_cout_line(simple_print::blue) << *t << " running...";
      simple_print::colored_cout_line(simple_print::blue) << simple_print::bar;
      try {
        t->m_passed = true;  // could be reset in the func
        t->m_func();
      } catch (assertion_fault) {
        t->m_passed = false;
      } catch (const std::exception& e) {
        t->m_passed = false;
        simple_print::colored_cout_line(simple_print::red) << *t << " raised " << e.what();
      } catch (...) {
        t->m_passed = false;
        simple_print::colored_cout_line(simple_print::red) << *t <<  " raised an exception";
      }

      if (t->m_passed) {
        num_passed++;
        simple_print::colored_cout_line(simple_print::green) << simple_print::bar;
        simple_print::colored_cout_line(simple_print::green) << *t << " PASSED";
      } else {
        num_failed++;
        simple_print::colored_cout_line(simple_print::red) << simple_print::bar;
        simple_print::colored_cout_line(simple_print::red) << *t << " FAILED";
      }

      show_green_assertions(old_green_assertions);
      current() = nullptr;
      simple_print::colored_cout_line(simple_print::normal) << "";
    }

    simple_print::colored_cout_line(simple_print::normal) << simple_print::barbar;
    if (num_passed) {
      simple_print::colored_cout_line(simple_print::green) << "passed:  " << num_passed;
    }
    if (num_failed) {
      simple_print::colored_cout_line(simple_print::red) << "failed:  " << num_failed;
      for (TestCase* t = first(); t; t = t->m_next) {
        if (t->m_called && !t->m_passed) {
          simple_print::colored_cout_line(simple_print::red) << " * " << *t;
        }
      }
    }

    if (num_skipped) {
      simple_print::colored_cout_line(simple_print::blue) << "skipped: " << num_skipped;
    }

    return !num_failed;
  }
};

inline void test_failed(bool assertion) {
  TestCase::current()->m_passed = false;
  if (assertion) throw assertion_fault{};
}

struct examination_afterword {
  bool passed;
  bool assertion;
  void operator <<= (auto&&) && {
    if (!passed) test_failed(assertion);
  }
};

inline constexpr auto get_color(bool passed, bool assertion) {
  return passed ? simple_print::green : assertion ? simple_print::red : simple_print::yellow;
}

// testing functions
inline bool expect_comparison(
    const char* file, int line,
    const char* aexpr, const auto& a,
    const char* bexpr, const auto& b,
    bool assertion,  // assert or expect?
    auto opfunc, const char* opexpr) {
  bool passed = opfunc(a, b);
  if (passed && !show_green_assertions()) return true;

  auto color = get_color(passed, assertion);
  const char* category = assertion ? "assertion" : "expectation";
  const char* verdict = passed ? "passed" : "failed";
  simple_print::colored_cout_line(color) << file << ":" << line;
  simple_print::colored_cout_line(color) << "  " << category << " " << verdict
      << ": " << aexpr << " " << opexpr << " " << bexpr;
  simple_print::colored_cout_line(color) << "    left : " << simple_print::verbose(a);
  simple_print::colored_cout_line(color) << "    right: " << simple_print::verbose(b);
  return passed;
}

inline bool examine_fault(const char* file, int line) {
  simple_print::colored_cout_line(simple_print::red) << file << ":" << line;
  simple_print::colored_cout_line(simple_print::red) << "  explicitly failed";
  return false;
}

inline int testing_main(int, char**) {
  return !simple_test::TestCase::run_all();
}

// comparisons

template<std::size_t N> struct compile_time_str {
    char buf[N] {};
    constexpr compile_time_str(const char(&b)[N]) {
        for (std::size_t i = 0; i != N; ++i) buf[i] = b[i];
    }
};
template<std::size_t N> compile_time_str(const char(&)[N]) -> compile_time_str<N>;

template<compile_time_str s> struct str_tag {};

// decorated name because it will be used outside the namespace
#define STR_TAG(op) ::simple_test::str_tag<#op>

template<class Tag> struct tagged_cmp;

#define TAGGED_CMP(op) tagged_cmp<STR_TAG(op)>

#define DECLARE_TAGGED_CMP(op) \
template<> struct TAGGED_CMP(op) { \
  constexpr auto operator()(const auto& a, const auto& b) const { return a op b; } \
};

DECLARE_TAGGED_CMP(==)
DECLARE_TAGGED_CMP(!=)
DECLARE_TAGGED_CMP(<)
DECLARE_TAGGED_CMP(>)
DECLARE_TAGGED_CMP(<=)
DECLARE_TAGGED_CMP(>=)

template<class Tag> struct tagged_strcmp {
  constexpr auto operator()(const auto& a, const auto& b) const {
    return tagged_cmp<Tag>()(std::strcmp(a, b), 0);
  }
};

#define TAGGED_STRCMP(op) tagged_strcmp<STR_TAG(op)>

// float comparison

template<class FLOAT> struct nearly_float {
  FLOAT value, epsilon;
  // this == x means x belongs to the neighourhood
  constexpr bool operator == (FLOAT x) const { return value-epsilon <= x && x <= value+epsilon; }
  constexpr bool operator != (FLOAT x) const { return !(*this == x); }
  // this < x means x is strictly greater than the upper bound
  constexpr bool operator <  (FLOAT x) const { return value+epsilon < x; }
  constexpr bool operator >  (FLOAT x) const { return x < value-epsilon; }
  // this <= x means x is greater-or-equal than the lower bound
  constexpr bool operator <= (FLOAT x) const { return value-epsilon <= x; }
  constexpr bool operator >= (FLOAT x) const { return x <= value+epsilon; }

  friend constexpr bool operator == (FLOAT x, const nearly_float& y) { return y == x; }
  friend constexpr bool operator != (FLOAT x, const nearly_float& y) { return y != x; }
  friend constexpr bool operator <  (FLOAT x, const nearly_float& y) { return y >  x; }
  friend constexpr bool operator >  (FLOAT x, const nearly_float& y) { return y <  x; }
  friend constexpr bool operator <= (FLOAT x, const nearly_float& y) { return y >= x; }
  friend constexpr bool operator >= (FLOAT x, const nearly_float& y) { return y <= x; }

  friend std::ostream& operator << (std::ostream& ost, const nearly_float& f) {
    return ost << f.value << " ± " << f.epsilon;
  }
};

template<class FLOAT, class EPS> constexpr auto nearly_abs(FLOAT v, EPS eps) {
  return nearly_float<FLOAT>{v, static_cast<FLOAT>(std::abs(eps))};
}
template<class FLOAT, class EPS> constexpr auto nearly_rel(FLOAT v, EPS eps) {
  return nearly_abs(v, v * eps);
}

template<class TAG, class EPS> struct tagged_floatcmp_factory {
  EPS eps;
  constexpr auto operator()(const auto& a, const auto& b) const {
    return tagged_cmp<TAG>()(nearly_abs(a, eps), b);
  }
};

#define TAGGED_FLOATCMP(op, eps) tagged_floatcmp<decltype(#op ## _op_tag), decltype(eps)>{eps}

}  // namespace simple_test

#define TEST(suite, name, ...) \
    void _test__##suite##__##name##__func(); \
    simple_test::TestCase _test__##suite##__##name##__var( \
        #suite, #name, \
        _test__##suite##__##name##__func ,##__VA_ARGS__); \
    void _test__##suite##__##name##__func() /* test body goes here */

#define EXAMINATION_SUFFIX(passed, assertion) \
    simple_test::examination_afterword{passed, assertion} <<= \
      simple_print::colored_cout_line(simple_test::get_color(passed, assertion)).ost()

#define EXAMINE_IMPL(ae, a, be, b, assertion, ...) \
    if (const bool passed = \
        simple_test::expect_comparison(__FILE__, __LINE__, ae, a, be, b, assertion, ##__VA_ARGS__); \
        passed && !simple_test::show_green_assertions()) ; \
    else EXAMINATION_SUFFIX(passed, assertion)
#define EXAMINE(a, b, assertion, ...) \
    EXAMINE_IMPL(#a, a, #b, b, assertion, ##__VA_ARGS__)

#define EXAMINE_CMP(a, op, b, assertion) \
    EXAMINE(a, b, assertion, simple_test::TAGGED_CMP(op)(), #op)
#define ASSERT_CMP(a, op, b) EXAMINE_CMP(a, op, b, true)
#define EXPECT_CMP(a, op, b) EXAMINE_CMP(a, op, b, false)

#define EXAMINE_STRCMP(a, op, b, assertion) \
    EXAMINE(a, b, assertion, simple_test::TAGGED_STRCMP(op)(), "[strcmp]" #op)
#define ASSERT_STRCMP(a, op, b) EXAMINE_STRCMP(a, op, b, true)
#define EXPECT_STRCMP(a, op, b) EXAMINE_STRCMP(a, op, b, false)

#define EXAMINE_BOOL(a, b, assertion) \
    EXAMINE(a, b, assertion, std::equal_to<bool>(), "is")
#define ASSERT_BOOL(a, b) EXAMINE_BOOL(a, b, true)
#define EXPECT_BOOL(a, b) EXAMINE_BOOL(a, b, false)

#define EXAMINE_FLOATCMP(a, op, b, eps, assertion) \
    EXAMINE_IMPL(#a, simple_test::nearly_abs(a, eps), #b, b, assertion, simple_test::TAGGED_CMP(op)(), "[near]" #op)
#define ASSERT_FLOATCMP(a, op, b, eps) EXAMINE_FLOATCMP(a, op, b, eps, true)
#define EXPECT_FLOATCMP(a, op, b, eps) EXAMINE_FLOATCMP(a, op, b, eps, false)

#define ASSERTION_FAULT(...) \
    if (simple_test::examine_fault(__FILE__, __LINE__, ##__VA_ARGS__)) ; \
    else EXAMINATION_SUFFIX(false, true)

#define TESTING_MAIN() \
    int main(int argc, char** argv) { return simple_test::testing_main(argc, argv); }

#define SIMPLE_TEST_PPCAT1(a, b) a ## b
#define SIMPLE_TEST_PPCAT(a, b) SIMPLE_TEST_PPCAT1(a, b)
#define SHOW_GREEN_ASSERTIONS(flag) \
    [[maybe_unused]] bool SIMPLE_TEST_PPCAT(_green_assertions_, __COUNTER__) = \
        simple_test::show_green_assertions(flag)

// GTest-like comparisons

#define ASSERT_EQ(a, b) ASSERT_CMP(a, ==, b)
#define ASSERT_NE(a, b) ASSERT_CMP(a, !=, b)
#define ASSERT_LT(a, b) ASSERT_CMP(a, <, b)
#define ASSERT_GT(a, b) ASSERT_CMP(a, >, b)
#define ASSERT_LE(a, b) ASSERT_CMP(a, <=, b)
#define ASSERT_GE(a, b) ASSERT_CMP(a, >=, b)
#define EXPECT_EQ(a, b) EXPECT_CMP(a, ==, b)
#define EXPECT_NE(a, b) EXPECT_CMP(a, !=, b)
#define EXPECT_LT(a, b) EXPECT_CMP(a, <, b)
#define EXPECT_GT(a, b) EXPECT_CMP(a, >, b)
#define EXPECT_LE(a, b) EXPECT_CMP(a, <=, b)
#define EXPECT_GE(a, b) EXPECT_CMP(a, >=, b)

#define ASSERT_STREQ(a, b) ASSERT_STRCMP(a, ==, b)
#define ASSERT_STRNE(a, b) ASSERT_STRCMP(a, !=, b)
#define EXPECT_STREQ(a, b) EXPECT_STRCMP(a, ==, b)
#define EXPECT_STRNE(a, b) EXPECT_STRCMP(a, !=, b)

#define ASSERT_TRUE(a)  ASSERT_BOOL(true, a)
#define ASSERT_FALSE(a) ASSERT_BOOL(false, a)
#define EXPECT_TRUE(a)  EXPECT_BOOL(true, a)
#define EXPECT_FALSE(a) EXPECT_BOOL(false, a)

#define ASSERT_NEAR(a, b, eps) ASSERT_FLOATCMP(a, ==, b, eps)
#define EXPECT_NEAR(a, b, eps) EXPECT_FLOATCMP(a, ==, b, eps)

#define FAIL() ASSERTION_FAULT()
