//===-- Functions for declaring bindings ----------------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//

#ifndef BINDINGS_H
#define BINDINGS_H

#include "curt/api_params.h"
#include "curt/fwd_windows.h"

namespace uiglue {

  void declareBindings(curt::HandleOr<HWND> view);

}

#endif
