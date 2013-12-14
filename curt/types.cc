//===-- Handle type implementations ---------------------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//

#include "types.h"

#include "include_windows.h"

namespace curt {
  namespace detail {
    void WindowTraits::release(HWND h) { DestroyWindow(h); }
    void FontTraits::release(HFONT h) { DeleteObject(h); }
  }
}
