#ifndef WIN_UTIL_H
#define WIN_UTIL_H

#include "fwd_windows.h"
#include <string>

namespace uiglue {
  namespace util {
    
    HINSTANCE thisModule();
    std::string wideToUtf8(std::wstring wide);
    std::wstring utf8ToWide(std::string utf8);
    std::wstring loadStringW(unsigned int resId);
    std::string loadString(unsigned int resId);
    void setWindowText(HWND wnd, std::string text);

  }
}

#endif
