#ifndef BUILTIN_CONTROLS_H
#define BUILTIN_CONTROLS_H

#include <string>
#include <WinUser.h>

namespace uiglue {

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

#endif
