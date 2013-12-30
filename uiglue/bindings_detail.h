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

#include "curt/fwd_windows.h"

#include <memory>
#include <string>

namespace uiglue {

class BindingHandlerCache;
class View;
struct ViewModelRef;

namespace detail {

void applyBindingsInner(std::unique_ptr<ViewModelRef> vmRef, HWND view);

struct MenuCommandT {};
struct ThisViewT {};

class BindingDecl {
  std::unique_ptr<View> m_viewData;
  HWND m_handle;

public:
  BindingDecl(HWND handle, BindingHandlerCache cache);
  ~BindingDecl();

  BindingDecl& operator()(ThisViewT, std::string binding, std::string value);
  BindingDecl& operator()(int ctrlId, std::string binding, std::string value);
  BindingDecl& operator()(MenuCommandT, int id, std::string handler);
};

} // end namespace detail

extern detail::MenuCommandT MenuCommand;
extern detail::ThisViewT ThisView;

} // end namespace uiglue

#endif
