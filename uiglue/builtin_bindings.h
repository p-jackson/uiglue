#ifndef BUILTIN_BINDINGS_H
#define BUILTIN_BINDINGS_H

#include "include_windows.h"
#include "observable.h"
#include "view.h"
#include "win_util.h"

#include <string>

using std::string;

namespace {

  void setWindowText(HWND wnd, uiglue::UntypedObservable observable) {
    auto stringObservable = observable.as<string>();
    auto text = stringObservable();
    SetWindowTextW(wnd, uiglue::util::utf8ToWide(text).c_str());
  }

}

namespace uiglue {
  namespace bindings {

    struct Text {
      static std::string name() {
        return { "text" };
      }

      static void init(HWND wnd, UntypedObservable observable, View&) {
        setWindowText(wnd, observable);
      }

      static void update(HWND wnd, UntypedObservable observable, View&) {
        setWindowText(wnd, observable);
      }
    };

    struct Value {
      static std::string name() {
        return { "value" };
      }

      static void init(HWND wnd, UntypedObservable observable, View& view) {
        setWindowText(wnd, observable);

        view.addCommandHandler(EN_CHANGE, wnd, [observable](HWND control) mutable {
          auto len = GetWindowTextLengthW(control);
          auto wide = std::wstring(len, 0);
          GetWindowTextW(control, &wide[0], len);
          auto stringObservable = observable.as<string>();
          stringObservable(util::wideToUtf8(wide));
        });
      }

      static void update(HWND wnd, UntypedObservable observable, View&) {
        setWindowText(wnd, observable);
      }
    };

  }
}

#endif
