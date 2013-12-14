#include "main_view.h"

#include "resource.h"

#include "curt/error.h"
#include "curt/include_windows.h"
#include "curt/util.h"

static const wchar_t* className = L"uiglue main view";

enum {
  NameLabel = 1,
  NameEdit,
  ModalButton,
  ShoutCheckbox,
  MessageLabel
};

namespace {
  HWND createCtrl(DWORD exstyle, LPCWSTR className, LPCWSTR title, DWORD style, HWND parent, int id) {
    auto x = CW_USEDEFAULT; // All dimensions are default
    auto menuOrId = reinterpret_cast<HMENU>(id);
    return CreateWindowExW(exstyle, className, title, style, x, x, x, x, parent, menuOrId, curt::thisModule(), nullptr);
  }
  HDWP moveCtrl(HDWP dwp, HWND wnd, int id, int x, int y, int w, int h) {
    return DeferWindowPos(dwp, GetDlgItem(wnd, id), nullptr, x, y, w, h, SWP_NOZORDER);
  }
  void setCtrlFont(HWND wnd, int id, curt::Font& font) {
    SendDlgItemMessageW(wnd, id, WM_SETFONT, reinterpret_cast<WPARAM>(font.get()), 1);
  }
  curt::Font defaultFont() {
    auto metrics = NONCLIENTMETRICSW{ sizeof(NONCLIENTMETRICSW) };
    SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, metrics.cbSize, &metrics, 0);
    return { CreateFontIndirectW(&metrics.lfMessageFont) };
  }
}

static void createControls(HWND wnd) {
  auto style = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS;

  createCtrl(0, L"STATIC", L"Name:", style, wnd, NameLabel);
  createCtrl(0, L"EDIT", L"", style | ES_LEFT | ES_AUTOHSCROLL | WS_BORDER, wnd, NameEdit);
  createCtrl(0, L"BUTTON", L"Modal", style | BS_PUSHBUTTON, wnd, ModalButton);
  createCtrl(0, L"BUTTON", L"Shout", style | BS_AUTOCHECKBOX, wnd, ShoutCheckbox);
  createCtrl(0, L"STATIC", L"Message", style, wnd, MessageLabel);

  static auto font = defaultFont();

  setCtrlFont(wnd, NameLabel, font);
  setCtrlFont(wnd, NameEdit, font);
  setCtrlFont(wnd, ModalButton, font);
  setCtrlFont(wnd, ShoutCheckbox, font);
  setCtrlFont(wnd, MessageLabel, font);
}

static void resizeWindow(HWND wnd, int w, int h) {
  const auto p = 11;
  const auto labelToEdit = 5;
  const auto editToButton = 9;

  auto dwp = BeginDeferWindowPos(5);

  auto y = p;
  dwp = moveCtrl(dwp, wnd, NameLabel, p, y, w - 2*p, 13);
  y += 13;

  y += labelToEdit;
  dwp = moveCtrl(dwp, wnd, NameEdit, p, y, w - 2*p - editToButton - 75, 23);
  dwp = moveCtrl(dwp, wnd, ModalButton, w - p - 75 - 1, y - 1, 75 + 2, 23 + 2);
  y += 23;

  y += p;
  dwp = moveCtrl(dwp, wnd, ShoutCheckbox, p, y, w - 2*p, 17);
  y += 17;

  y += p;
  dwp = moveCtrl(dwp, wnd, MessageLabel, p, y, w - 2*p, h - p - y);

  EndDeferWindowPos(dwp);
}

static LRESULT CALLBACK MainViewProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam) {
  try {
    curt::clearCurrentException();

    switch (msg) {
    case WM_CREATE:
      createControls(wnd);
      break;
    case WM_SIZE:
      resizeWindow(wnd, LOWORD(lParam), HIWORD(lParam));
      break;
    case WM_CTLCOLORBTN:
    case WM_CTLCOLORSTATIC:
      return reinterpret_cast<LRESULT>(GetStockObject(WHITE_BRUSH));
    case WM_DESTROY:
      PostQuitMessage(0);
      break;
    }

    return DefWindowProcW(wnd, msg, wParam, lParam);
  }                      
  catch (...) {
    curt::saveCurrentException();
    return 0;
  }
}

static bool isRegistered() {
  WNDCLASSEXW wcex;
  return GetClassInfoExW(curt::thisModule(), className, &wcex) != 0;
}

static void registerMainView() {
  auto wcex = WNDCLASSEXW{ sizeof(WNDCLASSEXW) };

  wcex.lpfnWndProc = &MainViewProc;
  wcex.hInstance = curt::thisModule();
  wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);
  wcex.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
  wcex.lpszClassName = className;
  wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_UIGLUE);
  wcex.hIcon = LoadIconW(curt::thisModule(), MAKEINTRESOURCEW(IDI_UIGLUE));
  wcex.hIconSm = LoadIconW(curt::thisModule(), MAKEINTRESOURCEW(IDI_SMALL));

  if (!RegisterClassExW(&wcex))
    throw std::runtime_error("Failed to register window class");
}

namespace uiglue {

curt::Window makeMainView() {
  if (!isRegistered())
    registerMainView();

  auto title = curt::loadStringW(IDS_APP_TITLE);
  auto x = CW_USEDEFAULT; // All dimensions are default

  return CreateWindowExW(WS_EX_APPWINDOW, className, title.c_str(), WS_OVERLAPPEDWINDOW, x, x, x, x, HWND_DESKTOP, nullptr, curt::thisModule(), nullptr);
}

}
