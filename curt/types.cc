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
    void HandleTraits<HWND>::release(HWND h) { DestroyWindow(h); }
    void HandleTraits<HFONT>::release(HFONT h) { DeleteObject(h); }
  }
}
