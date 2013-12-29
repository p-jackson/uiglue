//===-- Functions for declaring bindings ----------------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//

#ifndef UIGLUE_BINDINGS_H
#define UIGLUE_BINDINGS_H

#include "binding_handler_cache.h"
#include "make_unique.h"
#include "view_model_ref.h"

#include "curt/api_params.h"
#include "curt/fwd_windows.h"

#include <memory>
#include <string>

namespace uiglue {

class View;

void applyBindingsInner(std::unique_ptr<ViewModelRef> vmRef, HWND view);

struct MenuCommandT {};
extern MenuCommandT MenuCommand;

struct ThisViewT {};
extern ThisViewT ThisView;

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


BindingHandlerCache defaultBindingHandlers();

BindingDecl declareBindings(
  curt::HandleOr<HWND> view,
  BindingHandlerCache handlers = defaultBindingHandlers()
);

template<class ViewModel>
void applyBindings(ViewModel& vm, curt::HandleOr<HWND> view) {
  auto ref = detail::make_unique<ViewModelRefImpl<ViewModel>>(vm);
  applyBindingsInner(std::move(ref), view);
}

void detachViewModel(curt::HandleOr<HWND> view);

} // end namespace uiglue

#endif
