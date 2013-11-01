#include "view.h"

#include "include_windows.h"
#include "win_util.h"

#include <array>
#include <boost/algorithm/string/trim.hpp>

using namespace uiglue;
using std::string;
using std::wstring;

namespace {

  enum class BindType {
    Literal,
    Bind,
    Resource,
    File
  };

  const string k_bind = "bind:";
  const string k_resource = "resource:";
  const string k_file = "file:";
  const string k_literal = "literal:";

  BindType getBindingType(string s) {
    if (s.empty())
      return BindType::Literal;

    if (s.find(k_bind) == 0)
      return BindType::Bind;
    else if (s.find(k_resource) == 0)
      return BindType::Resource;
    else if (s.find(k_file) == 0)
      return BindType::File;
    else
      return BindType::Literal;
  }

  string stripBindingPrefix(BindType type, string s) {
    switch (type) {
    case BindType::Literal:
      if (s.find(k_literal) == 0)
        return boost::trim_copy(s.substr(k_literal.size()));
      return s;

    case BindType::Bind:
      return boost::trim_copy(s.substr(k_bind.size()));

    case BindType::Resource:
      return boost::trim_copy(s.substr(k_resource.size()));

    case BindType::File:
      return boost::trim_copy(s.substr(k_file.size()));

    default:
      BOOST_ASSERT(0);
      return {};
    }
  }

  UntypedObservable makeObservable(std::string binding, ViewModelRef& viewModel) {
    auto type = getBindingType(binding);
    auto value = stripBindingPrefix(type, binding);

    switch (type) {
    case BindType::Literal:
      return Observable<string>{ value }.asUntyped();

    case BindType::Bind:
      return viewModel.getObservable(value);

    default:
      BOOST_ASSERT(0);
      return Observable<string>().asUntyped();
    }
  }

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
    std::swap(o.m_childIds, m_childIds);
    std::swap(o.m_bindingDeclarations, m_bindingDeclarations);
    std::swap(o.m_bindingHandlers, m_bindingHandlers);

    SetWindowLongPtrW(m_wnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
  }

  View& View::operator=(View&& o) {
    std::swap(o.m_wnd, m_wnd);
    std::swap(o.m_font, m_font);
    std::swap(o.m_type, m_type);
    std::swap(o.m_vm, m_vm);
    std::swap(o.m_commands, m_commands);
    std::swap(o.m_childIds, m_childIds);
    std::swap(o.m_bindingDeclarations, m_bindingDeclarations);
    std::swap(o.m_bindingHandlers, m_bindingHandlers);

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

  void View::addCommandHandler(int commandCode, HWND control, std::function<void(HWND)> handler) {
    auto id = GetDlgCtrlID(control);
    if (!id)
      throw std::runtime_error("Couldn't find control id. Perhaps it wasn't created using uiglue.");

    auto key = ((commandCode << 16) | (id & 0xffff));
    m_commandHandlers[key].push_back(std::move(handler));
  }

  void View::addBindings(BindingDeclarations bindingDeclarations, BindingHandlers bindingHandlers) {
    m_bindingDeclarations = std::move(bindingDeclarations);
    m_bindingHandlers = std::move(bindingHandlers);
  }

  void View::addChildId(int id, std::string name) {
    m_childIds[std::move(name)] = id;
  }

  HFONT View::getFont() const {
    return m_font.get();
  }

  void View::detachViewModel() {
    m_vm.reset();
  }

  bool View::onMessage(unsigned int msg, WPARAM wParam, LPARAM lParam, LRESULT& result) {
    if (msg == WM_DESTROY && m_type == ViewType::App) {
      PostQuitMessage(0);
      return true;
    }

    if (msg == WM_COMMAND && HIWORD(wParam) == 0 && !lParam && m_vm) {
      // Handle menu commands
      auto id = LOWORD(wParam);
      auto found = m_commands.find(id);
      if (found != m_commands.end()) {
        m_vm->runCommand(found->second, *this);
        return true;
      }
    }

    if (msg == WM_COMMAND && lParam) {
      // Fire command handlers
      auto found = m_commandHandlers.find(wParam);
      if (found != end(m_commandHandlers)) {
        for (auto& handler : found->second)
          handler(reinterpret_cast<HWND>(lParam));
      }

      return true;
    }

    if (msg == WM_CTLCOLORBTN || msg == WM_CTLCOLOREDIT || msg == WM_CTLCOLORSTATIC) {
      result = reinterpret_cast<LRESULT>(GetStockObject(WHITE_BRUSH));
      return true;
    }

    return false;
  }

  void View::applyBindings() {
    for (auto& control : m_bindingDeclarations) {
      auto controlHandle = getChildControl(control.first);

      for (auto& binding : control.second) {
        auto handler = m_bindingHandlers.find(binding.first);
        if (handler == end(m_bindingHandlers))
          continue;

        auto observable = makeObservable(binding.second, *m_vm);
        handler->second->init(controlHandle, observable, *this);
      }
    }
  }

  HWND View::getChildControl(std::string name) const {
    auto found = m_childIds.find(name);
    if (found == end(m_childIds))
      throw std::runtime_error("Child control doesn't exist: " + name);

    auto handle = GetDlgItem(get(), found->second);

    if (!handle)
      throw std::runtime_error("Child control doesn't exist: " + name);

    return handle;
  }

}
