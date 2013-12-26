//===-- uiglue::Observable unit tests -------------------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//

#include "uiglue/observable.h"

#include <gtest/gtest.h>
#include <string>

using uiglue::Observable;

// Swallows the return statement in the FAIL macro
#define CONSTRUCTOR_FAIL() ([] { FAIL(); })()


TEST(Observable, ctorDefault) {
  auto o1 = Observable<std::string>{};
  ASSERT_EQ(std::string{}, o1());

  struct Obj {
    int count = 0;
    Obj() { ++count; }
  };
  auto o2 = Observable<Obj>{};
  ASSERT_EQ(1, o2().count);
}

TEST(Observable, ctorForwarding) {
  auto o1 = Observable<std::string>{ "test" };
  ASSERT_EQ("test", o1());

  auto o2 = Observable<std::pair<int, double>>{ 113, 3.14 };
  ASSERT_EQ(std::make_pair(113, 3.14), o2());

  struct Obj {
    int count;
    Obj(int c) : count{ c } {}
  };
  auto o3 = Observable<Obj>{ 3 };
  ASSERT_EQ(3, o3().count);
}

TEST(Observable, ctorCopy) {
  auto o1 = Observable<int>{ 113 };
  auto o2 = o1;
  ASSERT_EQ(113, o2());
  ASSERT_EQ(o1(), o2());

  static auto nextValue = 113;
  struct Obj {
    int value;
    Obj() : value{ nextValue++ } {}
  };
  auto o3 = Observable<Obj>{};
  ASSERT_EQ(113, o3().value);
  auto o4 = o3;
  ASSERT_EQ(113, o4().value);
  ASSERT_EQ(114, nextValue);
}

TEST(Observable, ctorMove) {
  auto o1 = Observable<int>{ 113 };
  auto o2 = std::move(o1);
  ASSERT_EQ(113, o2());

  static auto nextValue = 113;
  struct Obj {
    int value;
    Obj() : value{ nextValue++ } {}
    Obj(const Obj&) { CONSTRUCTOR_FAIL(); }
  };
  auto o3 = Observable<Obj>{};
  auto o4 = std::move(o3);
  ASSERT_EQ(113, o4().value);
  ASSERT_EQ(114, nextValue);
}

TEST(Observable, assignCopy) {
  Observable<int> o1;
  auto o2 = Observable<int>{ 113 };
  o1 = o2;
  ASSERT_EQ(113, o1());
  ASSERT_EQ(o1(), o2());

  static auto nextValue = 113;
  struct Obj {
    int value;
    Obj() : value{ nextValue++ } {}
  };
  Observable<Obj> o3; // This increments nextValue;
  auto o4 = Observable<Obj>{};
  ASSERT_EQ(114, o4().value);
  o3 = o4;
  ASSERT_EQ(114, o3().value);
  ASSERT_EQ(115, nextValue);
}

TEST(Observable, assignMove) {
  Observable<int> o1;
  auto o2 = Observable<int>{ 113 };
  o1 = std::move(o2);
  ASSERT_EQ(113, o1());

  static auto nextValue = 113;
  struct Obj {
    int value;
    Obj() : value{ nextValue++ } {}
    Obj(const Obj&) { CONSTRUCTOR_FAIL(); }
  };
  auto o3 = Observable<Obj>{};
  auto o4 = Observable<Obj>{};
  o3 = std::move(o4);
  ASSERT_EQ(114, o3().value);
  ASSERT_EQ(115, nextValue);
}

TEST(Observable, get) {
  auto o1 = Observable<int>{ 113 };
  ASSERT_EQ(113, o1());

  auto o2 = Observable<std::tuple<int, int, int>>{ 1, 2, 3 };
  ASSERT_EQ(std::make_tuple(1, 2, 3), o2());

  // Test that the getter returns a modifiable reference
  std::get<0>(o2()) = 2;
  std::get<1>(o2()) = 4;
  std::get<2>(o2()) = 6;
  ASSERT_EQ(std::make_tuple(2, 4, 6), o2());
}

TEST(Observable, set) {
  auto o1 = Observable<int>{ 113 };
  ASSERT_EQ(113, o1());
  o1(114);
  ASSERT_EQ(114, o1());
  o1(2 * o1());
  ASSERT_EQ(228, o1());

  static auto nextValue = 113;
  struct Obj {
    int value;
    Obj() : value{ nextValue++ } {}
    bool operator==(const Obj& o) { return value == o.value; }
  };
  auto obj = Obj{};
  auto o2 = Observable<Obj>{};
  ASSERT_EQ(113, obj.value);
  ASSERT_EQ(114, o2().value);
  ASSERT_EQ(115, nextValue);

  o2(std::move(obj));
  ASSERT_EQ(113, o2().value);
  ASSERT_EQ(115, nextValue);
}
