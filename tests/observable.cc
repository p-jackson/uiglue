//===-- uiglue::Observable unit tests -------------------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//

#include "uiglue/observable.h"

#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <string>

using uiglue::Observable;

// Swallows the return statement in the FAIL macro
#define CONSTRUCTOR_FAIL() \
  ([] { FAIL(); })()


TEST_CASE(
    "An Observable<T> is default constructible if T is default constructible",
    "[observable]") {
  // The wrapped object equals a default constructed object
  auto o1 = Observable<std::string>{};
  REQUIRE(o1() == std::string{});

  // The wrapped object's default constructor will be called
  struct Obj {
    int count = 0;
    Obj() {
      ++count;
    }
  };
  auto o2 = Observable<Obj>{};
  REQUIRE(o2().count == 1);
}

TEST_CASE(
    "Observables forward constructor arguments to the wrapped object's "
    "constructor",
    "[observable]") {
  // Forwards a single argument
  auto o1 = Observable<std::string>{ "test" };
  REQUIRE(o1() == "test");

  // Forwards multiple arguments
  auto o2 = Observable<std::tuple<int, double, float>>{ 113, 3.14, 6.28f };
  REQUIRE(o2() == std::make_tuple(113, 3.14, 6.28f));

  // Forwards moved objects without copying
  struct Obj {
    bool copied = false;
    Obj() = default;
    Obj(const Obj&) : copied{ true } {
    }
    // MSVC doesn't support default'd move ctors yet
    Obj(Obj&& o) : copied{ o.copied } {
    }
  };

  auto obj = Obj{};
  auto o3 = Observable<Obj>{ std::move(obj) };
  REQUIRE_FALSE(o3().copied);
}

TEST_CASE("An Observable<T> is copy constructible if T is copy constructible",
          "[observable]") {
  auto o1 = Observable<int>{ 113 };
  auto o2 = o1;
  REQUIRE(o2() == 113);
  REQUIRE(o2() == o1());

  static auto nextValue = 113;
  struct Obj {
    int value;
    Obj() : value{ nextValue++ } {
    }
  };
  auto o3 = Observable<Obj>{};
  REQUIRE(o3().value == 113);
  auto o4 = o3;
  REQUIRE(o4().value == 113);
  REQUIRE(nextValue == 114);
}

TEST_CASE("Observables are move constructible", "[observable]") {
  auto o1 = Observable<int>{ 113 };
  auto o2 = std::move(o1);
  REQUIRE(o2() == 113);

  static auto nextValue = 113;
  struct Obj {
    int value;
    Obj() : value{ nextValue++ } {
    }
    Obj(const Obj&) {
      CONSTRUCTOR_FAIL();
    }
  };
  auto o3 = Observable<Obj>{};
  auto o4 = std::move(o3);
  REQUIRE(o4().value == 113);
  REQUIRE(nextValue == 114);
}

TEST_CASE("An Observable<T> is copy assignable if T is copyable",
          "[observable]") {
  Observable<int> o1;
  auto o2 = Observable<int>{ 113 };
  o1 = o2;
  REQUIRE(o1() == 113);
  REQUIRE(o2() == o1());

  static auto nextValue = 113;
  struct Obj {
    int value;
    Obj() : value{ nextValue++ } {
    }
  };
  Observable<Obj> o3; // This increments nextValue;
  auto o4 = Observable<Obj>{};
  REQUIRE(o4().value == 114);
  o3 = o4;
  REQUIRE(o3().value == 114);
  REQUIRE(nextValue == 115);
}

TEST_CASE("Observables are move assignable", "[observable]") {
  Observable<int> o1;
  auto o2 = Observable<int>{ 113 };
  o1 = std::move(o2);
  REQUIRE(o1() == 113);

  static auto nextValue = 113;
  struct Obj {
    int value;
    Obj() : value{ nextValue++ } {
    }
    Obj(const Obj&) {
      CONSTRUCTOR_FAIL();
    }
  };
  auto o3 = Observable<Obj>{};
  auto o4 = Observable<Obj>{};
  o3 = std::move(o4);
  REQUIRE(o3().value == 114);
  REQUIRE(nextValue == 115);
}

TEST_CASE("Observables can return wrapped value types", "[observable]") {
  auto o = Observable<int>{ 113 };
  REQUIRE(o() == 113);

  SECTION("the accessor returns a modifiable reference") {
    o() = 114;
    REQUIRE(o() == 114);
  }
}

TEST_CASE("Observables can return wrapped non-trivial data types",
          "[observable]") {
  auto o = Observable<std::tuple<int, int, int>>{ 1, 2, 3 };
  REQUIRE(o() == std::make_tuple(1, 2, 3));

  SECTION("the accessor returns a modifiable reference") {
    std::get<0>(o()) = 2;
    std::get<1>(o()) = 4;
    std::get<2>(o()) = 6;
    REQUIRE(o() == std::make_tuple(2, 4, 6));
  }
}

TEST_CASE("Observables can set the wrapped data types", "[observable]") {
  auto o1 = Observable<int>{ 113 };
  REQUIRE(o1() == 113);
  o1(114);
  REQUIRE(o1() == 114);
  o1(2 * o1());
  REQUIRE(o1() == 228);

  static auto nextValue = 113;
  struct Obj {
    int value;
    Obj() : value{ nextValue++ } {
    }
    bool operator==(const Obj& o) {
      return value == o.value;
    }
  };
  auto obj = Obj{};
  auto o2 = Observable<Obj>{};
  REQUIRE(obj.value == 113);
  REQUIRE(o2().value == 114);
  REQUIRE(nextValue == 115);

  o2(std::move(obj));
  REQUIRE(o2().value == 113);
  REQUIRE(nextValue == 115);
}

TEST_CASE("Can be notified of changes to the wrapped object", "[observable]") {
  auto o1Count = 0;
  auto o1 = Observable<int>{ 113 };
  o1.subscribe([&o1Count](int v) {
    ++o1Count;
    REQUIRE(v == 114);
  });
  o1(114);
  REQUIRE(o1() == 114);
  REQUIRE(o1Count == 1);
  o1(114); // Shouldn't call the subscribed function a second time
  REQUIRE(o1() == 114);
  REQUIRE(o1Count == 1);

  // Again with strings
  auto o2Count = 0;
  auto o2 = Observable<std::string>{ "a" };
  o2.subscribe([&o2Count](std::string s) {
    ++o2Count;
    REQUIRE(s == "b");
  });
  o2("b");
  REQUIRE(o2() == "b");
  REQUIRE(o2Count == 1);
  o2("b"); // Shouldn't call the subscribed function a second time
  REQUIRE(o2() == "b");
  REQUIRE(o2Count == 1);
}

TEST_CASE(
    "Can unsubscribe to no longer be notified of changes to the wrapped object",
    "[observable]") {
  auto o = Observable<int>{ 113 };

  auto count1 = 0;
  auto sub1 = o.subscribe([&count1](int v) {
    ++count1;
    REQUIRE(v == 114);
  });
  o(114);
  REQUIRE(o() == 114);
  REQUIRE(count1 == 1);

  o.unsubscribe(sub1);
  o(115);
  REQUIRE(count1 == 1);
}

TEST_CASE("Can unsubscribe from an observable in a different order",
          "[observable]") {
  auto o = Observable<std::string>{ "a" };

  auto count1 = 0;
  auto sub1 = o.subscribe([&count1](std::string) { ++count1; });

  auto count2 = 0;
  o.subscribe([&count2](std::string) { ++count2; });

  o("b");
  REQUIRE(count1 == 1);
  REQUIRE(count2 == 1);

  o.unsubscribe(sub1);
  o("c");
  REQUIRE(count1 == 1);
  REQUIRE(count2 == 2);
}

TEST_CASE("Can unsubscribe within the change handler", "[observable]") {
  auto o = Observable<int>{ 113 };

  auto count = 0;
  int sub;
  sub = o.subscribe([&](int) {
    ++count;
    o.unsubscribe(sub);
  });

  o(114);
  REQUIRE(count == 1);

  o(115);
  REQUIRE(count == 1);
}

TEST_CASE("The wrapped object can be modified from within a change handler",
          "[observable]") {
  auto o = Observable<int>{ 113 };

  auto count = 0;
  o.subscribe([&](int v) {
    ++count;
    if (count == 1)
      o(v + 1);
  });

  o(114);
  REQUIRE(o() == 115);
  REQUIRE(count == 2);
}
