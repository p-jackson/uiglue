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
      static string name() {
        return { "text" };
      }

      static void init(HWND wnd, UntypedObservable observable, View&) {
        setWindowText(wnd, observable);
      }

      static void update(HWND wnd, UntypedObservable observable, View&) {
        setWindowText(wnd, observable);
      }
    };


    struct Title : public Text {
      static string name() {
        return { "title" };
      }
    };


    struct Value {
      static string name() {
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


    struct Visible {
      static string name() {
        return { "visible" };
      }

      static void init(HWND wnd, UntypedObservable observable, View&) {
        ShowWindow(wnd, showWindowCmd(observable));
      }

      static void update(HWND wnd, UntypedObservable observable, View&) {
        ShowWindow(wnd, showWindowCmd(observable));
      }

      static int showWindowCmd(UntypedObservable observable) {
        auto boolObservable = observable.as<bool>();
        return boolObservable() ? SW_SHOWNA : SW_HIDE;
      }
    };


    struct Hidden {
      static string name() {
        return { "hidden" };
      }

      static void init(HWND wnd, UntypedObservable observable, View&) {
        ShowWindow(wnd, showWindowCmd(observable));
      }

      static void update(HWND wnd, UntypedObservable observable, View&) {
        ShowWindow(wnd, showWindowCmd(observable));
      }

      static int showWindowCmd(UntypedObservable observable) {
        auto boolObservable = observable.as<bool>();
        return boolObservable() ? SW_HIDE : SW_SHOWNA;
      }
    };


    struct Checked {
      static string name() {
        return { "checked" };
      }

      static void init(HWND wnd, UntypedObservable observable, View& view) {
        SendMessageW(wnd, BM_SETCHECK, buttonState(observable), 0);

        view.addCommandHandler(BN_CLICKED, wnd, [observable](HWND control) mutable {
          auto state = SendMessageW(control, BM_GETCHECK, 0, 0);
          if (observable.is<int>()) {
            auto intObservable = observable.as<int>();
            intObservable(static_cast<int>(state));
          }
          else {
            auto boolObservable = observable.as<bool>();
            boolObservable(state == BST_CHECKED);
          }
        });
      }

      static void update(HWND wnd, UntypedObservable observable, View&) {
        SendMessageW(wnd, BM_SETCHECK, buttonState(observable), 0);
      }

      static int buttonState(UntypedObservable observable) {
        if (observable.is<int>()) {
          auto intObservable = observable.as<int>();
          switch (intObservable()) {
          case 0: return BST_UNCHECKED;
          case 1: return BST_CHECKED;
          default: return BST_INDETERMINATE;
          }
        }
        else {
          auto boolObservable = observable.as<bool>();
          return boolObservable() ? BST_CHECKED : BST_UNCHECKED;
        }
      }
    };

  }
}

#endif
