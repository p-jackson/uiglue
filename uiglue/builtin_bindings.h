#ifndef BUILTIN_BINDINGS_H
#define BUILTIN_BINDINGS_H

#include "observable.h"
#include "win_util.h"

#include <string>

namespace uiglue {
  namespace bindings {

    struct Text {
      static std::string name() {
        return { "text" };
      }

      static void init(HWND wnd, UntypedObservable observable) {
        update(wnd, observable);
      }

      static void update(HWND wnd, UntypedObservable observable) {
        auto stringObservable = observable.as<std::string>();
        auto s = stringObservable();
        SetWindowTextW(wnd, util::utf8ToWide(s).c_str());
      }
    };

  }
}

#endif
