//===-- BindingHandler class template -------------------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//

#ifndef UIGLUE_BINDING_HANDLER_H
#define UIGLUE_BINDING_HANDLER_H

#include "observable.h"

#include "curt/fwd_windows.h"

#include <string>

namespace uiglue {

class UntypedObservable;
class View;

struct IBindingHandler {
  virtual ~IBindingHandler() {}
  IBindingHandler() = default;
  IBindingHandler(const IBindingHandler&) = delete;
  IBindingHandler& operator=(const IBindingHandler&) = delete;

  virtual std::string name() const = 0;
  virtual void init(HWND ctrl, UntypedObservable ob, View& view) const = 0;
  virtual void update(HWND ctrl, UntypedObservable ob, View& view) const = 0;
};

namespace detail {

  template<class A, A> struct Helper {};

  using BindingFunc = void (*)(HWND, UntypedObservable, View&);

  template<class T>
  struct HasInit {
    template<class A>
    std::true_type static test(A*, Helper<BindingFunc, &A::init>* = nullptr);
    std::false_type static test(...);

    using type = decltype(test(static_cast<T*>(nullptr)));
  };

  template<class T>
  struct HasUpdate {
    template<class A>
    std::true_type static test(A*, Helper<BindingFunc, &A::update>* = nullptr);
    std::false_type static test(...);

    using type = decltype(test(static_cast<T*>(nullptr)));
  };

  // Methods for BindingHandler that don't depend on the template argument.
  class BindingHandlerNoTmpl {
  public:
    void doInit(std::false_type, HWND, UntypedObservable, View&) const {}
    void doUpdate(std::false_type, HWND, UntypedObservable, View&) const {}
  };

}

template<class Traits>
class BindingHandler
  : public IBindingHandler,
    public detail::BindingHandlerNoTmpl
{
public:
  std::string name() const override {
    return Traits::name();
  }

  void init(HWND h, UntypedObservable o, View& v) const override {
    using HasInit = typename detail::HasInit<Traits>::type;
    doInit(HasInit{}, h, std::move(o), v);
  }

  void update(HWND h, UntypedObservable o, View& v) const override {
    using HasUpdate = typename detail::HasUpdate<Traits>::type;
    doUpdate(HasUpdate{}, h, std::move(o), v);
  }

private:
  // Ensure these methods participate in overload resolution
  using detail::BindingHandlerNoTmpl::doInit;
  using detail::BindingHandlerNoTmpl::doUpdate;

  void doInit(std::true_type, HWND h, UntypedObservable o, View& v) const {
    Traits::init(h, std::move(o), v);
  }

  void doUpdate(std::true_type, HWND h, UntypedObservable o, View& v) const {
    Traits::update(h, std::move(o), v);
  }
};

} // end namespace uiglue

#endif
