//===-- Main view ---------------------------------------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//

#include "main_view.h"

#include "resource.h"

#include "curt/curt.h"
#include "curt/error.h"
#include "curt/include_windows.h"
#include "curt/util.h"
#include "uiglue/bindings.h"

enum {
  NameLabel = 1,
  NameEdit,
  ModalButton,
  ShoutCheckbox,
  MessageLabel
};

static HDWP moveCtrl(HDWP dwp, HWND wnd, int id, int x, int y, int w, int h) {
  auto ctrl = curt::getDlgItem(wnd, id);
  return DeferWindowPos(dwp, ctrl, nullptr, x, y, w, h, SWP_NOZORDER);
}

static void createControls(HWND wnd) {
  curt::createStatic(wnd, NameLabel);
  curt::createEdit(wnd, NameEdit);
  curt::createButton(wnd, ModalButton);
  curt::createCheckbox(wnd, ShoutCheckbox);
  curt::createStatic(wnd, MessageLabel);

  curt::setControlToDefaultFont(wnd, NameLabel);
  curt::setControlToDefaultFont(wnd, NameEdit);
  curt::setControlToDefaultFont(wnd, ModalButton);
  curt::setControlToDefaultFont(wnd, ShoutCheckbox);
  curt::setControlToDefaultFont(wnd, MessageLabel);
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

static LRESULT CALLBACK MainViewProc(
  HWND wnd,
  UINT msg,
  WPARAM wParam,
  LPARAM lParam
) {
  try {
    switch (msg) {
    case WM_CREATE:
      createControls(wnd);
      break;
    case WM_SIZE:
      resizeWindow(wnd, LOWORD(lParam), HIWORD(lParam));
      break;
    }

    return curt::defWindowProc(wnd, msg, wParam, lParam);
  }                      
  catch (...) {
    curt::saveCurrentException();
    return 0;
  }
}

static ATOM registerMainView() {
  auto wcex = WNDCLASSEXW{ sizeof(WNDCLASSEXW) };

  wcex.lpfnWndProc = &MainViewProc;
  wcex.hInstance = curt::thisModule();
  wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);
  wcex.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
  wcex.lpszClassName = L"uiglue main view";
  wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_UIGLUE);
  wcex.hIcon = LoadIconW(curt::thisModule(), MAKEINTRESOURCEW(IDI_UIGLUE));
  wcex.hIconSm = LoadIconW(curt::thisModule(), MAKEINTRESOURCEW(IDI_SMALL));

  return curt::registerClassEx(&wcex);
}

namespace uiglue {

curt::Window makeMainView() {
  static auto classAtom = registerMainView();

  const auto x = CW_USEDEFAULT; // All dimensions are default

  auto mainView = curt::createWindowEx(
    WS_EX_APPWINDOW,
    classAtom,
    nullptr,
    WS_OVERLAPPEDWINDOW,
    x, x, x, x,
    HWND_DESKTOP,
    nullptr,
    curt::thisModule(),
    nullptr
  );

  curt::subclassControlBackground(mainView, RGB(255, 255, 255));
  curt::subclassAppView(mainView);

  declareBindings(mainView)
    (uiglue::ThisView, "text", curt::loadString(IDS_APP_TITLE))
    (uiglue::MenuCommand, IDM_ABOUT, "onAbout")
    (uiglue::MenuCommand, IDM_EXIT, "onExit")
    (NameLabel, "text", "Name:")
    (NameEdit, "value", "bind: name")
    (ModalButton, "text", "Modal")
    (ModalButton, "click", "onModalGreeting")
    (ShoutCheckbox, "text", "Shout")
    (ShoutCheckbox, "checked", "bind: shout")
    (MessageLabel, "text", "bind: greeting")
  ;

  return mainView;
}

} // end namespace uiglue
