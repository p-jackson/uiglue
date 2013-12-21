//===-- Functions for declaring bindings ----------------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//

#include "bindings.h"

#include "binding_handler_cache.h"
#include "builtin_bindings.h"
#include "view.h"
#include "view_messages.h"

#include "curt/curt.h"

using namespace std;
using curt::HandleOr;

namespace uiglue {

BindingDeclaration::BindingDeclaration(HWND h, BindingHandlerCache cache)
  : viewData{ std::make_unique<View>(h) },
    handle{ h }
{
  viewData->addBindingHandlerCache(std::move(cache));
}

BindingDeclaration::~BindingDeclaration() {
  auto viewDataAsInt = reinterpret_cast<uintptr_t>(viewData.get());
  curt::setWindowSubclass(handle, &View::WndProc, 0, viewDataAsInt);

  // Ownership of viewData has now passed to the view
  viewData.release();
}

BindingDeclaration& BindingDeclaration::operator()(
  int controlId,
  std::string binding,
  std::string value
) {
  viewData->addBinding(controlId, binding, value);
  return *this;
}

BindingDeclaration declareBindings(HandleOr<HWND> view, BindingHandlerCache cache) {
  return { view, std::move(cache) };
}

void detachViewModel(HandleOr<HWND> view) {
  static auto msg = curt::registerWindowMessage(detachVMMsg);
  curt::sendMessage(static_cast<HWND>(view), msg, 0, 0);
}

void applyBindingsInner(unique_ptr<ViewModelRef> vmRef, HWND view) {
  static auto msg = curt::registerWindowMessage(applyBindingsMsg);
  auto asLParam = reinterpret_cast<LPARAM>(vmRef.get());
  curt::sendMessage(view, msg, 0, asLParam);
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
