//===-- Functions for declaring bindings ----------------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//

#include "bindings.h"

#include "builtin_bindings.h"
#include "view.h"
#include "view_messages.h"

#include "curt/curt.h"

using namespace std;
using curt::HandleOr;

namespace uiglue {

BindingDecl::BindingDecl(HWND handle, BindingHandlerCache cache)
  : m_viewData{ make_unique<View>(handle) },
    m_handle{ handle }
{
  m_viewData->addBindingHandlerCache(move(cache));
}

BindingDecl::~BindingDecl() {
  auto asInt = reinterpret_cast<uintptr_t>(m_viewData.get());
  curt::setWindowSubclass(m_handle, &View::WndProc, 0, asInt);

  // Ownership of m_viewData has been passed to the view
  m_viewData.release();
}

BindingDecl& BindingDecl::operator()(int ctrlId, string binding, string value) {
  m_viewData->addBinding(ctrlId, binding, value);
  return *this;
}

BindingDecl declareBindings(HandleOr<HWND> view, BindingHandlerCache cache) {
  return { view, move(cache) };
}

void detachViewModel(HandleOr<HWND> view) {
  static auto msg = curt::registerWindowMessage(detachVMMsg);
  curt::sendMessage(static_cast<HWND>(view), msg, 0, 0);
}

void applyBindingsInner(unique_ptr<ViewModelRef> vmRef, HWND view) {
  static auto msg = curt::registerWindowMessage(applyBindingsMsg);
  auto asLParam = reinterpret_cast<LPARAM>(vmRef.get());
  curt::sendMessage(view, msg, 0, asLParam);

  // Ownership of vmRef has been passed to the view
  vmRef.release();
}

BindingHandlerCache defaultBindingHandlers() {
  auto cache = BindingHandlerCache{};

  cache.addBindingHandler<BuiltinBinding<bindings::Text>>();
  cache.addBindingHandler<BuiltinBinding<bindings::Title>>();
  cache.addBindingHandler<BuiltinBinding<bindings::Value>>();
  cache.addBindingHandler<BuiltinBinding<bindings::Visible>>();
  cache.addBindingHandler<BuiltinBinding<bindings::Hidden>>();
  cache.addBindingHandler<BuiltinBinding<bindings::Checked>>();
  cache.addBindingHandler<BuiltinBinding<bindings::Click>>();

  return cache;
}

} // end namespace uiglue
