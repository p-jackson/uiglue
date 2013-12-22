//===-- Handles for Windows types -----------------------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//

#ifndef CURT_TYPES_H
#define CURT_TYPES_H

#include "fwd_windows.h"
#include "handle.h"

namespace curt {
  
namespace detail {
  template<typename T>
  struct HandleTraits {
    using Type = void;
    static bool valid();
    static void release();
  };
}


namespace detail {
  template<>
  struct HandleTraits<HWND> {
    using Type = HWND;
    static bool valid(HWND h) { return h != nullptr; }
    static void release(HWND h);
  };
}

using Window = Handle<detail::HandleTraits<HWND>>;


namespace detail {
  template<>
  struct HandleTraits<HFONT> {
    using Type = HFONT;
    static bool valid(HFONT h) { return h != nullptr; }
    static void release(HFONT h);
  };
}

using Font = Handle<detail::HandleTraits<HFONT>>;

} // end namespace curt

#endif
