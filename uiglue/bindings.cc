//===-- Functions for declaring bindings ----------------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//

#include "bindings.h"

#include "view.h"

#include "curt/curt.h"

#include <memory>

namespace uiglue {

void declareBindings(curt::HandleOr<HWND> view) {
  auto viewData = std::make_unique<View>();
  auto viewDataPtr = reinterpret_cast<std::uintptr_t>(viewData.get());
  curt::setWindowSubclass(static_cast<HWND>(view), &View::WndProc, 0, viewDataPtr);

  // Ownership of viewData has now passed to the view
  viewData.release();
}

} // end namespace uiglue
