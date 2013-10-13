#ifndef WIN_TYPES_H
#define WIN_TYPES_H

#include "include_windows.h"

namespace uiglue {

  template<class Traits>
  class Handle {
    typedef typename Traits::Type Type;
    Type handle;

  public:
    Handle() : Handle(nullptr) {
    }

    Handle(Type h) : handle(h) {
    }

    Handle(Handle&& other) : Handle() {
      using std::swap;
      swap(other.handle, handle);
    }

    ~Handle() {
      if (handle)
        Traits::destroy(handle);
    }

    Handle& operator=(Handle&& other) {
      using std::swap;
      swap(other.handle, handle);
      return *this;
    }

    auto get() const -> decltype(handle) {
      return handle;
    }

  private:
    Handle(Handle&) = delete;
    Handle& operator=(Handle&) = delete;
  };

  struct FontTraits {
    typedef HFONT Type;
    static void destroy(HFONT h) { DeleteObject(h); }
  };

  typedef Handle<FontTraits> Font;

};

#endif
