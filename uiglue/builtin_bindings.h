//===-- Bindings that come with uiglue ------------------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//

#ifndef BUILTIN_BINDINGS_H
#define BUILTIN_BINDINGS_H

#include "observable.h"
#include "view.h"

#include "curt/include_windows.h"
#include "curt/win_util.h"

#include <string>

using std::string;

namespace uiglue {

  template<class Traits>
  class BuiltinBinding : public Binding {
  public:
    std::string name() const override {
      return Traits::name();
    }

    void init(HWND wnd, UntypedObservable observable, View& view) const override {
      Traits::init(wnd, std::move(observable), view);
    }

    void update(HWND wnd, UntypedObservable observable, View& view) const override {
      Traits::update(wnd, std::move(observable), view);
    }
  };

  namespace bindings {

    inline string getWindowText(HWND wnd) {
      auto len = GetWindowTextLengthW(wnd);
      auto wide = std::wstring(len + 1, 0);
      GetWindowTextW(wnd, &wide[0], len + 1);
      return uiglue::util::wideToUtf8(std::move(wide));
    }

    inline void setWindowText(HWND wnd, uiglue::UntypedObservable observable, bool checkFirst = false) {
      auto stringObservable = observable.as<string>();
      auto text = stringObservable();
      if (checkFirst && text == getWindowText(wnd))
        return;
      SetWindowTextW(wnd, uiglue::util::utf8ToWide(text).c_str());
    }

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
          auto text = getWindowText(control);
          auto stringObservable = observable.as<string>();
          stringObservable(text);
        });
      }

      static void update(HWND wnd, UntypedObservable observable, View&) {
        setWindowText(wnd, observable, true);
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

    struct Click {
      static string name() {
        return { "click" };
      }

      static void init(HWND wnd, UntypedObservable observable, View& view) {
        auto stringObservable = observable.as<string>();
        view.addCommandHandler(BN_CLICKED, wnd, stringObservable());
      }

      static void update(HWND, UntypedObservable, View&) {
      }
    };

  }
}

#endif
