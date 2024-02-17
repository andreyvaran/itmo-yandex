#include "event.hpp"
#include "event.hpp"

#include <cassert>
#include <string>

#include "../simple_test.h"

TEST(event, no_callbacks) {
  cls11::Event<void(const char*)> ev;
  ev.fire("test");
}

TEST(event, single_callback) {
  cls11::Event<void(const char*)> ev;

  std::string s;
  int invoked = 0;
  ev.subscribe([&](const char* t) { s = t; ++invoked; });

  ev.fire("test");
  EXPECT_EQ("test", s);
  EXPECT_EQ(1, invoked);

  invoked = 0;
  ev.fire("next");
  EXPECT_EQ("next", s);
  EXPECT_EQ(1, invoked);
}

TEST(event, same_callback_added_twice) {
  cls11::Event<void(const char*)> ev;

  std::string s;
  int invoked = 0;

  auto f = [&](const char* t) { s = t; ++invoked; };
  ev.subscribe(f);
  ev.subscribe(f);

  ev.fire("test");
  EXPECT_EQ("test", s);
  EXPECT_EQ(2, invoked);

  invoked = 0;

  ev.fire("next");
  EXPECT_EQ("next", s);
  EXPECT_EQ(2, invoked);
}

TEST(event, proper_call_order) {
  cls11::Event<void(const char*)> ev;

  int invoked = 0;

  ev.subscribe([&](auto) {
    EXPECT_EQ(0, invoked);
    ++invoked;
  });
  ev.subscribe([&](auto) {
    EXPECT_EQ(1, invoked);
    ++invoked;
  });
  ev.subscribe([&](auto) {
    EXPECT_EQ(2, invoked);
    ++invoked;
  });

  ev.fire("test");
  EXPECT_EQ(3, invoked);

  invoked = 0;
  ev.fire("next");
  EXPECT_EQ(3, invoked);
}

TEST(event, polymorph) {
  cls11::Event<void(int)> ev;

  int invoked = 0;
  ev.subscribe([&](int x) {
    EXPECT_EQ(-5, x);
    ++invoked;
  });
  ev.subscribe([&](unsigned char x) {
    EXPECT_EQ(251, +x);
    ++invoked;
  });
  ev.subscribe([&](bool x) {
    EXPECT_EQ(true, x);
    ++invoked;
  });

  ev.fire(-5);
  EXPECT_EQ(3, invoked);
}

TEST(event, nonvoid) {
  cls11::Event<int(int)> ev;

  int invoked = 0;

  ev.subscribe([&](auto) {
    EXPECT_EQ(0, invoked);
    return ++invoked;
  });
  ev.subscribe([&](auto) {
    EXPECT_EQ(1, invoked);
    return ++invoked;
  });
  ev.subscribe([&](auto) {
    EXPECT_EQ(2, invoked);
    return ++invoked;
  });

  ev.fire(0);
  EXPECT_EQ(3, invoked);
}

TEST(event, reference) {
  cls11::Event<void(int&)> ev;

  ev.subscribe([](int& x) {
    EXPECT_EQ(0, x);
    x = 100;
  });
  ev.subscribe([](int& x) {
    EXPECT_EQ(100, x);
    x = 200;
  });
  ev.subscribe([](int& x) {
    EXPECT_EQ(200, x);
    x = 300;
  });

  int invoked = 0;

  ev.fire(invoked);
  EXPECT_EQ(300, invoked);
}

TESTING_MAIN()
