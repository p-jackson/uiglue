//===-- Controls that come with uiglue ------------------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//

#ifndef BUILTIN_CONTROLS_H
#define BUILTIN_CONTROLS_H

#include "view_factory.h"

#include <string>
#include <WinUser.h>

namespace uiglue {

  template<class Traits>
  class BuiltinControlFactory : public ControlFactory {
  public:
    std::string name() const override {
      return Traits::name();
    }

    HWND create(HWND parent, int ctrlId) const override {
      auto style = Traits::style() | WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS;
      auto exStyle = Traits::exStyle();
      auto cls = Traits::className();

      auto x = 0;
      auto y = (ctrlId - 1) * 50;
      auto cx = 300;
      auto cy = 50;

      adjustInitialPosition(x, y, cx, cy);

      auto control = CreateWindowExW(exStyle, cls.c_str(), nullptr, style, x, y, cx, cy, parent, reinterpret_cast<HMENU>(ctrlId), util::thisModule(), nullptr);

      if (!control)
        throw std::runtime_error("Failed to create child control: " + name());

      return control;
    }

  protected:
    virtual void adjustInitialPosition(int&, int&, int&, int&) const {}
  };

  namespace controls {

    struct Static {
      static std::string name() { return { "Static" }; }
      static unsigned int style() { return 0; }
      static unsigned int exStyle() { return 0; }
      static std::wstring className() { return { L"STATIC" }; }
    };

    struct Edit {
      static std::string name() { return { "Edit" }; }
      static unsigned int style() { return ES_LEFT | ES_AUTOHSCROLL | WS_BORDER; }
      static unsigned int exStyle() { return 0; }
      static std::wstring className() { return { L"EDIT" }; }
    };

    struct Button {
      static std::string name() { return { "Button" }; }
      static unsigned int style() { return BS_PUSHBUTTON; }
      static unsigned int exStyle() { return 0; }
      static std::wstring className() { return { L"BUTTON" }; }
    };

    struct Checkbox {
      static std::string name() { return { "Checkbox" }; }
      static unsigned int style() { return BS_AUTOCHECKBOX; }
      static unsigned int exStyle() { return 0; }
      static std::wstring className() { return { L"BUTTON" }; }
    };

  }
}

#endif
