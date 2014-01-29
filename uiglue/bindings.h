//===-- Functions for declaring bindings ----------------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//

#ifndef UIGLUE_BINDINGS_H
#define UIGLUE_BINDINGS_H

#include "bindings_detail.h"
#include "binding_handler_cache.h"
#include "make_unique.h"
#include "view_model_ref.h"

#include "curt/api_params.h"
#include "curt/fwd_windows.h"

#include <algorithm>

namespace uiglue {

BindingHandlerCache defaultBindingHandlers();

detail::BindingDecl declareBindings(curt::HandleOr<HWND> view,
                                    BindingHandlerCache handlers
                                    = defaultBindingHandlers());

void applyBindings(std::unique_ptr<IViewModelRef> vmRef, HWND view);

template <class ViewModel>
void applyBindings(ViewModel& vm, curt::HandleOr<HWND> view) {
  auto ref = detail::make_unique<ViewModelRef<ViewModel>>(vm);
  applyBindings(std::move(ref), view);
}

void detachViewModel(curt::HandleOr<HWND> view);

} // end namespace uiglue

#endif
