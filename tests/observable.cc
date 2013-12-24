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

TEST(Observable, ctorCopy) {
  auto o1 = Observable<int>{ 113 };
  auto o2 = o1;
  ASSERT_EQ(113, o2());
  ASSERT_EQ(o1(), o2());

  struct Obj {
    bool copied = false;
    Obj() = default;
    Obj(const Obj&) : copied{ true } { }
    bool operator==(const Obj&) { return false; }
  };
  auto o3 = Observable<Obj>{};
  ASSERT_FALSE(o3().copied);
  auto o4 = o3;
  ASSERT_TRUE(o4().copied);
}
