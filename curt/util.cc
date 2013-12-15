//===-- Definition of Win32 wrapper functions -----------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//

#include "util.h"

#include "curt.h"
#include "error.h"
#include "include_windows.h"

using std::string;
using std::wstring;

extern "C" IMAGE_DOS_HEADER __ImageBase;

namespace curt {

HINSTANCE thisModule() {
  return reinterpret_cast<HINSTANCE>(&__ImageBase);
}

string wideToUtf8(wstring wide) {
  if (wide.empty())
    return {};

  if (wide.size() > std::numeric_limits<int>::max())
    throw std::runtime_error("String too long to convert to UTF-8");

  const auto nChar = static_cast<int>(wide.size());
  const auto flags = WC_ERR_INVALID_CHARS;

  const auto nBytes = wideCharToMultiByte(CP_UTF8, flags, wide.data(), nChar, nullptr, 0, nullptr, nullptr);

  auto narrow = string(nBytes, '\0');
  wideCharToMultiByte(CP_UTF8, flags, wide.data(), nChar, &narrow[0], nBytes, nullptr, nullptr);

  return narrow;
}

wstring utf8ToWide(string utf8) {
  if (utf8.empty())
    return {};

  if (utf8.size() > std::numeric_limits<int>::max())
    throw std::runtime_error("String too long to convert to wide chars");

  const auto nBytes = static_cast<int>(utf8.size());
  const auto flags = MB_ERR_INVALID_CHARS;

  const auto nChar = multiByteToWideChar(CP_UTF8, flags, utf8.data(), nBytes, nullptr, 0);

  auto wide = std::wstring(nChar, '\0');
  multiByteToWideChar(CP_UTF8, flags, utf8.data(), nBytes, &wide[0], nChar);

  return wide;
}

wstring loadStringW(unsigned int resId) {
  wchar_t* buff;
  auto len = curt::loadString(thisModule(), resId, reinterpret_cast<wchar_t*>(&buff), 0);
  return wstring(buff, len);
}

string loadString(unsigned int resId) {
  return wideToUtf8(loadStringW(resId));
}

WPARAM pumpMessages() {
  MSG msg;
  while (getMessage(&msg, nullptr, 0, 0)) {
    translateMessage(&msg);
    dispatchMessage(&msg);

    throwSavedException();
  }

  return msg.wParam;
}

WPARAM pumpMessages(HandleOr<HWND> translateWnd, HACCEL accelTable) {
  MSG msg;
  while (getMessage(&msg, nullptr, 0, 0)) {
    if (!TranslateAcceleratorW(translateWnd, accelTable, &msg)) {
      translateMessage(&msg);
      dispatchMessage(&msg);
        
      throwSavedException();
    }
  }

  return msg.wParam;
}

} // end namespace curt
