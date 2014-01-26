//===-- View creation -----------------------------------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//

#ifndef DIALOG_DIALOG_H
#define DIALOG_DIALOG_H

#include "curt/types.h"

namespace dialogExample {

curt::Window createMainView();
curt::Window createSliderView(HWND parent);

} // end namespace dialogExample

#endif
