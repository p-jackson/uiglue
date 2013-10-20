#include "view.h"

#include "include_windows.h"
#include "win_util.h"

#include <array>

using namespace uiglue;
using std::string;
using std::wstring;

namespace {

  unsigned long typeToStyle(ViewType t) {
    switch (t) {
    case ViewType::App:
      return WS_OVERLAPPEDWINDOW;
    default:
      throw std::runtime_error("Invalid view type");
    }
  }

  unsigned long typeToStyleEx(ViewType t) {
    switch (t) {
    case ViewType::App:
      return WS_EX_APPWINDOW;
    default:
      return 0;
    }
  }

  HWND createWindow(wstring className, ViewType type, View* view) {
    const auto stylex = typeToStyleEx(type);
    const auto style = typeToStyle(type);
    const auto dim = CW_USEDEFAULT;

    auto wnd = CreateWindowExW(stylex, className.c_str(), nullptr, style, dim, dim, dim, dim, nullptr, nullptr, uiglue::util::thisModule(), view);

    if (!wnd)
      throw std::runtime_error("Failed to create view: " + util::wideToUtf8(className));

    return wnd;
  }

  Font defaultFont() {
    auto metrics = NONCLIENTMETRICSW{ sizeof(NONCLIENTMETRICSW) };
    SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, metrics.cbSize, &metrics, 0);
    return { CreateFontIndirectW(&metrics.lfMessageFont) };
  }


}

namespace uiglue {

  std::exception_ptr View::s_lastError;

  std::exception_ptr View::getLastError() {
    return s_lastError;
  }

  LRESULT __stdcall View::WndProc(HWND wnd, unsigned int msg, WPARAM w, LPARAM l) {
    try {
      s_lastError = {};

      if (msg == WM_NCCREATE) {
        auto create = reinterpret_cast<CREATESTRUCTW*>(l);
        auto viewPtr = reinterpret_cast<View*>(create->lpCreateParams);
        SetWindowLongPtrW(wnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(viewPtr));
        viewPtr->m_wnd = wnd;
      }

      auto t = reinterpret_cast<View*>(GetWindowLongPtrW(wnd, GWLP_USERDATA));
      if (!t)
        return DefWindowProcW(wnd, msg, w, l);

      auto result = LRESULT{ 0 };
      if (!t->onMessage(msg, w, l, result))
        result = DefWindowProcW(wnd, msg, w, l);

      if (msg == WM_NCCREATE)
        t->m_wnd = nullptr;

      return result;
    }
    catch (...) {
      s_lastError = std::current_exception();
      return 0;
    }
  }

  View::View()
    : m_wnd{ nullptr }
  {
  }

  View::View(string className, ViewType type)
    : m_wnd{ createWindow(util::utf8ToWide(className), type, this) },
      m_font{ defaultFont() },
      m_type{ type }
  {
  }

  View::View(View&& o)
    : m_wnd{ nullptr },
      m_type{ ViewType::App }
  {
    std::swap(o.m_wnd, m_wnd);
    std::swap(o.m_font, m_font);
    std::swap(o.m_type, m_type);
    std::swap(o.m_vm, m_vm);
    std::swap(o.m_commands, m_commands);

    SetWindowLongPtrW(m_wnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
  }

  View& View::operator=(View&& o) {
    std::swap(o.m_wnd, m_wnd);
    std::swap(o.m_font, m_font);
    std::swap(o.m_type, m_type);
    std::swap(o.m_vm, m_vm);
    std::swap(o.m_commands, m_commands);

    SetWindowLongPtrW(m_wnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

    return *this;
  }

  View::~View() {
    if (m_wnd)
      DestroyWindow(m_wnd);
  }

  HWND View::get() const {
    return m_wnd;
  }

  void View::addCommand(int id, std::string command) {
    m_commands[id] = command;
  }

  HFONT View::getFont() const {
    return m_font.get();
  }

  void View::detachViewModel() {
    m_vm.reset();
  }

  bool View::onMessage(unsigned int msg, WPARAM wParam, LPARAM, LRESULT& result) {
    if (msg == WM_DESTROY && m_type == ViewType::App) {
      PostQuitMessage(0);
      return true;
    }

    if (msg == WM_COMMAND && HIWORD(wParam) == 0 && m_vm) {
      // Handle menu commands
      auto id = LOWORD(wParam);
      auto found = m_commands.find(id);
      if (found != m_commands.end()) {
        m_vm->runCommand(found->second, *this);
        return true;
      }
    }

    if (msg == WM_CTLCOLORBTN || msg == WM_CTLCOLOREDIT || msg == WM_CTLCOLORSTATIC) {
      result = reinterpret_cast<LRESULT>(GetStockObject(WHITE_BRUSH));
      return true;
    }

    return false;
  }

}
