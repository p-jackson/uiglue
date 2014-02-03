//===-- Functions for declaring bindings ----------------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//
//
// Some of these functions send custom messages to the view. The custom message
// ids are defined using registerWindowMessage(). This is usually used for
// inter-process communication, but is used here to prevent clashes with ids in
// client code.
//
// This file also defines all the default binding handlers.
//
//===----------------------------------------------------------------------===//

#include "bindings.h"

#include "binding_handler.h"
#include "make_unique.h"
#include "observable.h"
#include "view.h"
#include "view_messages.h"

#include "curt/curt.h"
#include "curt/include_windows.h"
#include "curt/util.h"

#include <CommCtrl.h>

using namespace std;
using namespace curt;
using namespace uiglue;

// The following namespace contains the built-in binding handlers.
namespace {

// Sets the window text using SetWindowText()
struct Text {
  static string name() {
    return { "text" };
  }

  static void update(HWND wnd, ValueAccessor accessor, View&) {
    curt::setWindowText(wnd, accessor.as<string>());
  }
};

// Alias for `text`
struct Title : public Text {
  static string name() {
    return { "title" };
  }
};

// Two-way binding for edit and slider controls
struct Value {
  static string name() {
    return { "value" };
  }

  static void init(HWND wnd, ValueAccessor accessor, View& view) {
    if (accessor.isWriteableObservable<string>()) {

      auto changeHandler = [accessor](HWND control) mutable {
        auto text = getWindowTextString(control);
        auto observable = accessor.asWriteableObservable<string>();
        observable(text);
      };

      view.addCommandHandler(EN_CHANGE, wnd, changeHandler);
    } else if (accessor.isWriteableObservable<int>()) {

      auto scrollHandler = [accessor, wnd](WPARAM, LPARAM lParam) mutable {
        if (wnd != reinterpret_cast<HWND>(lParam))
          return;

        auto pos = sendMessage(wnd, TBM_GETPOS, 0, 0);
        auto observable = accessor.asWriteableObservable<int>();
        observable(static_cast<int>(pos));
      };

      view.addMessageHandler(WM_VSCROLL, scrollHandler);
      view.addMessageHandler(WM_HSCROLL, scrollHandler);
    }
  }

  static void update(HWND wnd, ValueAccessor accessor, View&) {
    if (accessor.is<string>()) {
      auto text = accessor.as<string>();
      if (text != curt::getWindowTextString(wnd))
        curt::setWindowText(wnd, text);
    } else {
      auto value = accessor.as<int>();
      sendMessage(wnd, TBM_SETPOS, 1, value);
    }
  }
};

// Sets the window visibility using ShowWindow()
struct Visible {
  static string name() {
    return { "visible" };
  }

  static void update(HWND wnd, ValueAccessor accessor, View&) {
    auto visible = accessor.as<bool>();
    curt::showWindow(wnd, visible ? SW_SHOWNA : SW_HIDE);
  }
};

// Sets the window visibility using ShowWindow()
// Same as Visible, just opposite polarity
struct Hidden {
  static string name() {
    return { "hidden" };
  }

  static void update(HWND wnd, ValueAccessor accessor, View&) {
    auto hidden = accessor.as<bool>();
    curt::showWindow(wnd, hidden ? SW_HIDE : SW_SHOWNA);
  }
};

// Two-way binding for checkbox buttons
struct Checked {
  static string name() {
    return { "checked" };
  }

  static void init(HWND wnd, ValueAccessor accessor, View& view) {
    if (!accessor.isWriteableObservable<bool>()
        && !accessor.isWriteableObservable<int>())
      return;

    view.addCommandHandler(BN_CLICKED, wnd, [accessor](HWND control) mutable {
      auto state = curt::sendMessage(control, BM_GETCHECK, 0, 0);
      if (accessor.isWriteableObservable<int>()) {
        auto asInt = accessor.asWriteableObservable<int>();
        asInt(static_cast<int>(state));
      } else {
        auto boolObservable = accessor.asWriteableObservable<bool>();
        boolObservable(state == BST_CHECKED);
      }
    });
  }

  static void update(HWND wnd, ValueAccessor accessor, View&) {
    int state;
    if (accessor.is<int>())
      state = accessor.as<int>();
    else
      state = accessor.as<bool>() ? BST_CHECKED : BST_UNCHECKED;

    curt::sendMessage(wnd, BM_SETCHECK, state, 0);
  }
};

// Sets up a handler for a BN_CLICKED command
struct Click {
  static string name() {
    return { "click" };
  }

  static void init(HWND wnd, ValueAccessor accessor, View& view) {
    auto commandName = accessor.as<string>();
    view.addCommandHandler(BN_CLICKED, wnd, commandName);
  }
};

// Sets the minimum for a slider control
struct Min {
  static string name() {
    return { "min" };
  }

  static void update(HWND wnd, ValueAccessor accessor, View&) {
    auto min = accessor.as<int>();
    sendMessage(wnd, TBM_SETRANGEMIN, 1, min);
  }
};

// Sets the maximum for a slider control
struct Max {
  static string name() {
    return { "max" };
  }

  static void update(HWND wnd, ValueAccessor accessor, View&) {
    auto max = accessor.as<int>();
    sendMessage(wnd, TBM_SETRANGEMAX, 1, max);
  }
};

// Binds a child view to a child view model
struct With {
  static std::string name() {
    return { "with" };
  }

  static void update(HWND view, ValueAccessor accessor, View&) {
    auto vm = accessor.asViewModelRef();
    uiglue::applyBindings(std::move(vm), view);
  }
};

} // end namespace


namespace uiglue {

// Defines keywords used in the binding DSL
detail::MenuCommandT MenuCommand;
detail::ThisViewT ThisView;

namespace detail {

  BindingDecl::BindingDecl(HWND handle, BindingHandlerCache cache)
      : m_viewData{ detail::make_unique<View>(handle) }, m_handle{ handle } {
    m_viewData->addBindingHandlerCache(move(cache));
  }

  // Once the binding declarations are complete the window is subclassed by View
  // so it can respond to commands and call the binding handlers.
  BindingDecl::~BindingDecl() {
    auto asInt = reinterpret_cast<uintptr_t>(m_viewData.get());
    setWindowSubclass(m_handle, &View::WndProc, 0, asInt);

    // Ownership of m_viewData has been passed to the view
    m_viewData.release();
  }

  // Handles menu commands
  BindingDecl& BindingDecl::operator()(MenuCommandT, int id, string handler) {
    m_viewData->addMenuCommand(id, handler);
    return *this;
  }

} // end namespace detail


// Starts the binding DSL by returning a BindingDecl.
detail::BindingDecl declareBindings(HandleOr<HWND> view,
                                    BindingHandlerCache cache) {
  return { view, move(cache) };
}


// Applies bindings to a view model already wrapped in a ViewModelRef.
// Uses a window message to pass ownership of the ViewModelRef to the view.
void applyBindings(unique_ptr<IViewModelRef> vmRef, HWND view) {
  static auto msg = registerWindowMessage(applyBindingsMsg);
  auto asLParam = reinterpret_cast<LPARAM>(vmRef.get());
  sendMessage(view, msg, 0, asLParam);

  // Ownership of vmRef has been passed to the view
  vmRef.release();
}


// Sends a message to the view subclass.
void detachViewModel(HandleOr<HWND> view) {
  static auto msg = registerWindowMessage(detachVMMsg);
  sendMessage(view, msg, 0, 0);
}


BindingHandlerCache defaultBindingHandlers() {
  auto cache = BindingHandlerCache{};

  cache.addBindingHandler<Text>();
  cache.addBindingHandler<Title>();
  cache.addBindingHandler<Value>();
  cache.addBindingHandler<Visible>();
  cache.addBindingHandler<Hidden>();
  cache.addBindingHandler<Checked>();
  cache.addBindingHandler<Click>();
  cache.addBindingHandler<Min>();
  cache.addBindingHandler<Max>();
  cache.addBindingHandler<With>();

  return cache;
}

} // end namespace uiglue
