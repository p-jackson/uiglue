//===-- Non-public functions for declaring bindings -----------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//
//
// These declarations are necessary for bindings.h but have been put in a
// different header separate them from the public interface.
//
// Implementations are in bindings.cc
//
//===----------------------------------------------------------------------===//

#ifndef UIGLUE_BINDINGS_DETAIL_H
#define UIGLUE_BINDINGS_DETAIL_H

#include "observable.h"
#include "view.h"

#include "curt/fwd_windows.h"

#include <memory>
#include <string>
#include <type_traits>

namespace uiglue {

class BindingHandlerCache;
struct ViewModelRef;

namespace detail {

struct MenuCommandT {};
struct ThisViewT {};

class BindingDecl {
  std::unique_ptr<View> m_viewData;
  HWND m_handle;

public:
  BindingDecl(HWND handle, BindingHandlerCache cache);
  ~BindingDecl();

  template<class T>
  BindingDecl& operator()(ThisViewT, std::string handler, T t) {
    using IsStringish = std::is_convertible<T, std::string>;
    addViewBinding(IsStringish{}, move(handler), std::forward<T>(t));
    return *this;
  }

  template<class T>
  BindingDecl& operator()(int ctrlId, std::string handler, T t) {
    using IsStringish = std::is_convertible<T, std::string>;
    addCtrlBinding(IsStringish{}, ctrlId, move(handler), std::forward<T>(t));
    return *this;
  }

  BindingDecl& operator()(MenuCommandT, int id, std::string handler);

private:
  template<class T>
  void addViewBinding(std::true_type, std::string handler, T t) {
    auto s = std::string{ t };
    m_viewData->addViewBinding(move(handler), move(s));
  }

  template<class T>
  void addViewBinding(std::false_type, std::string handler, T t) {
    auto observable = Observable<T>{ t };
    m_viewData->addViewBinding(move(handler), observable.asUntyped());
  }

  template<class T>
  void addCtrlBinding(std::true_type, int id, std::string handler, T t) {
    auto s = std::string{ t };
    m_viewData->addControlBinding(id, move(handler), move(s));
  }

  template<class T>
  void addCtrlBinding(std::false_type, int id, std::string handler, T t) {
    auto observable = Observable<T>{ t };
    m_viewData->addControlBinding(id, move(handler), observable.asUntyped());
  }
};

} // end namespace detail

extern detail::MenuCommandT MenuCommand;
extern detail::ThisViewT ThisView;

} // end namespace uiglue

#endif
