#include "win_util.h"

#include "include_windows.h"

using std::string;
using std::wstring;

extern "C" IMAGE_DOS_HEADER __ImageBase;

static void throwConversionError() {
  switch (GetLastError()) {
  case ERROR_NO_UNICODE_TRANSLATION:
    throw std::invalid_argument("Invalid input character");
  default:
    throw std::runtime_error("Failed to convert to UTF-8");
  }
}

namespace uiglue { namespace util {

  HINSTANCE thisModule() {
    return reinterpret_cast<HINSTANCE>(&__ImageBase);
  }

  string wideToUtf8(wstring wide) {
    if (wide.size() > std::numeric_limits<int>::max())
      throw std::runtime_error("String too long to convert to UTF-8");

    const auto nChar = static_cast<int>(wide.size());
    const auto flags = WC_ERR_INVALID_CHARS;

    const auto nBytes = WideCharToMultiByte(CP_UTF8, flags, wide.data(), nChar, nullptr, 0, nullptr, nullptr);

    if (!nBytes)
      throwConversionError();

    auto narrow = string(nBytes, '\0');
    if (!WideCharToMultiByte(CP_UTF8, flags, wide.data(), nChar, &narrow[0], nBytes, nullptr, nullptr))
      throwConversionError();

    return narrow;
  }

  wstring utf8ToWide(string utf8) {
    if (utf8.size() > std::numeric_limits<int>::max())
      throw std::runtime_error("String too long to convert to wide chars");

    const auto nBytes = static_cast<int>(utf8.size());
    const auto flags = MB_ERR_INVALID_CHARS;

    const auto nChar = MultiByteToWideChar(CP_UTF8, flags, utf8.data(), nBytes, nullptr, 0);

    if (!nChar)
      throwConversionError();

    auto wide = std::wstring(nChar, '\0');
    if (!MultiByteToWideChar(CP_UTF8, flags, utf8.data(), nBytes, &wide[0], nChar))
      throwConversionError();

    return wide;
  }

  wstring loadStringW(unsigned int resId) {
    wchar_t* buff;
    auto len = LoadStringW(thisModule(), resId, reinterpret_cast<wchar_t*>(&buff), 0);
    return wstring(buff, len);
  }

  string loadString(unsigned int resId) {
    return wideToUtf8(loadStringW(resId));
  }

  void setWindowText(HWND wnd, string text) {
    auto asWide = util::utf8ToWide(text);
    SetWindowTextW(wnd, asWide.c_str());
  }

} }
