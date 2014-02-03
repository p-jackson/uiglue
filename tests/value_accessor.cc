//===-- uiglue::ValueAccessor unit tests ----------------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//

#include "uiglue/value_accessor.h"

#include "uiglue/member_map.h"

#include <catch.hpp>

using namespace uiglue;

TEST_CASE("ValueAccessor is constructible", "[accessor]") {
  auto o = Observable<std::string>{};
  REQUIRE(o().empty());

  auto untyped = o.asUntyped();
  REQUIRE(untyped.is<std::string>());

  SECTION("from copied UntypedObservable") {
    REQUIRE_NOTHROW(auto accessor = ValueAccessor{ untyped };);
  }

  SECTION("from moved UntypedObservable") {
    REQUIRE_NOTHROW(auto accessor = ValueAccessor{ std::move(untyped) };);
  }
}

TEST_CASE("Can test ValueAccessor type", "[accessor]") {
  auto string = Observable<std::string>{};
  auto constString = Observable<const std::string>{};

  auto accessor = ValueAccessor{ string.asUntyped() };
  auto constAccessor = ValueAccessor{ constString.asUntyped() };

  SECTION("using same type returns true") {
    REQUIRE(accessor.is<std::string>());
    REQUIRE(constAccessor.is<const std::string>());
  }

  SECTION("using incorrect type returns false") {
    REQUIRE_FALSE(accessor.is<int>());
    REQUIRE_FALSE(constAccessor.is<const int>());
  }

  SECTION("cv-qualifications are ignored") {
    REQUIRE(accessor.is<const std::string>());
    REQUIRE(accessor.is<volatile std::string>());
    REQUIRE(constAccessor.is<std::string>());
    REQUIRE(constAccessor.is<volatile std::string>());
  }

  SECTION("l-value references are ignored") {
    REQUIRE(accessor.is<std::string&>());
    REQUIRE(constAccessor.is<const std::string&>());
  }
}

TEST_CASE("ValueAccessor returns the value", "[accessor]") {
  auto observable = Observable<std::string>{ "test" };
  auto accessor = ValueAccessor{ observable.asUntyped() };
  REQUIRE(observable() == "test");
  REQUIRE(accessor.is<std::string>());

  SECTION("returns a copy") {
    auto value = accessor.as<std::string>();
    REQUIRE(value == "test");
    observable("changed");
    REQUIRE(observable() == "changed");
    REQUIRE(value == "test");
  }

  SECTION("throw bad_cast if access incorrect type") {
    REQUIRE_FALSE(accessor.is<int>());
    REQUIRE_THROWS_AS(accessor.as<int>(), std::bad_cast);
  }
}

TEST_CASE("Can test whether ValueAccessor is writable", "[accessor]") {
  auto string = Observable<std::string>{};
  auto accessor = ValueAccessor{ string.asUntyped() };

  SECTION("using correct type") {
    REQUIRE(accessor.isWriteableObservable<std::string>());
  }

  SECTION("using incorrect type") {
    REQUIRE_FALSE(accessor.isWriteableObservable<int>());
  }
}

TEST_CASE("ValueAccessor can return observable", "[accessor]") {
  auto string = Observable<std::string>{};
  auto accessor = ValueAccessor{ string.asUntyped() };
  REQUIRE(accessor.isWriteableObservable<std::string>());

  SECTION("write to returned observable") {
    auto observable = accessor.asWriteableObservable<std::string>();
    observable("test");
    REQUIRE(observable() == "test");
    REQUIRE(string() == "test");
  }

  SECTION("write to incorrect type") {
    REQUIRE_FALSE(accessor.isWriteableObservable<int>());
    REQUIRE_THROWS_AS(accessor.asWriteableObservable<int>(), std::bad_cast);
  }
}

struct ViewModel {
  UIGLUE_BEGIN_MEMBER_MAP(ViewModel)
  UIGLUE_END_MEMBER_MAP()
};

TEST_CASE("ValueAccessor can test whether value is a view model",
          "[accessor]") {
  SECTION("correctly detects view model") {
    auto vm = Observable<ViewModel>{};
    auto vmAccessor = ValueAccessor{ vm.asUntyped() };
    REQUIRE(vmAccessor.isViewModel());
  }

  SECTION("correctly detects non-view model") {
    auto i = Observable<int>{};
    auto intAccessor = ValueAccessor{ i.asUntyped() };
    REQUIRE_FALSE(intAccessor.isViewModel());
  }
}

TEST_CASE("ValueAccessor can return view model reference", "[accessor]") {
  SECTION("return view model reference") {
    auto vm = Observable<ViewModel>{};
    auto vmAccessor = ValueAccessor{ vm.asUntyped() };
    REQUIRE_NOTHROW(vmAccessor.asViewModelRef());
  }

  SECTION("throws when accessing non-view model") {
    auto i = Observable<int>{};
    auto intAccessor = ValueAccessor{ i.asUntyped() };
    REQUIRE_FALSE(intAccessor.isViewModel());
    REQUIRE_THROWS_AS(intAccessor.asViewModelRef(), std::bad_cast);
  }
}
