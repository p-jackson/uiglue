//===-- Handle class declaration ------------------------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//
//
// Handle is a resource managing type that implements RAII for Windows handles.
// This type implements the move-only semantics, and the Traits type defines
// the specifics e.g. HWNDs are released with DestroyWindow(), but HFONTs are
// released with DeleteObject().
//
// The traits type should have the following interface:
//
// struct TraitsType {
//   // Native Win32 handle type e.g. HWND, HFONT, etc.
//   using Type = `handle type`;
//
//   // Return true if t has been initialised
//   static bool valid(Type t)
//
//   // Release t using the appropriate Win32 function
//   static void release(Type t)
// };
//
//===----------------------------------------------------------------------===//

#ifndef CURT_HANDLE_H
#define CURT_HANDLE_H

namespace curt {

template<class Type, template<class> class TraitsAccessor>
class Handle {
  using Traits = TraitsAccessor<Type>;
  Type handle;

public:
  Handle() : Handle(nullptr) {
  }

  Handle(Type h) : handle(h) {
  }

  Handle(const Handle&) = delete;
  Handle& operator=(const Handle&) = delete;

  Handle(Handle&& other) : Handle(nullptr) {
    using std::swap;
    swap(other.handle, handle);
  }

  ~Handle() {
    if (Traits::valid(handle))
      Traits::release(handle);
  }

  Handle& operator=(Handle&& other) {
    using std::swap;
    swap(other.handle, handle);
    return *this;
  }

  Type get() const {
    return handle;
  }

  explicit operator bool() const {
    return Traits::valid(handle);
  }
};

} // end namespace curt

#endif
