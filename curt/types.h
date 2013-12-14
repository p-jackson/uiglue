//===-- Handles for Windows types -----------------------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//

#ifndef CURT_WIN_TYPES_H
#define CURT_WIN_TYPES_H

#include "fwd_windows.h"
#include "handle.h"

namespace curt {
  
namespace detail {
  struct WindowTraits {
    using Type = HWND;
    static bool valid(HWND h) { return h != nullptr; }
    static void release(HWND h);
  };
}

using Window = Handle<detail::WindowTraits>;


namespace detail {
  struct FontTraits {
    using Type = HFONT;
    static bool valid(HFONT h) { return h != nullptr; }
    static void release(HFONT h);
  };
}

using Font = Handle<detail::FontTraits>;

} // end namespace curt

#endif
