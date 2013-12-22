//===-- Main view for hello_world -----------------------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//

#include "view.h"

#include "curt/curt.h"
#include "curt/include_windows.h"
#include "curt/util.h"
#include "uiglue/bindings.h"

#include <windowsx.h>

enum ControlIds {
  Message
};

bool onCreate(HWND wnd, LPCREATESTRUCT) {
  curt::createStatic(wnd, Message, SS_CENTER | SS_CENTERIMAGE);
  curt::setControlToDefaultFont(wnd, Message);
  return true;
}

void onSize(HWND wnd, UINT, int w, int h) {
  SetWindowPos(curt::getDlgItem(wnd, Message), nullptr, 0, 0, w, h, SWP_NOZORDER);
}

void onDestroy(HWND) {
  PostQuitMessage(0);
}

LRESULT CALLBACK viewProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam) {
  switch (msg) {
    HANDLE_MSG(wnd, WM_CREATE, onCreate);
    HANDLE_MSG(wnd, WM_SIZE, onSize);
    HANDLE_MSG(wnd, WM_DESTROY, onDestroy);
    default:
      return DefWindowProc(wnd, msg, wParam, lParam);
  }
}

ATOM registerWindowClass() {
  WNDCLASSEX wc = { sizeof(WNDCLASSEX) };
  wc.hInstance = curt::thisModule();
  wc.lpfnWndProc = viewProc;
  wc.lpszClassName = L"mainview";
  wc.style = CS_VREDRAW | CS_HREDRAW;
  return RegisterClassEx(&wc);
}

curt::Window createView() {
  static auto atom = registerWindowClass();

  const auto x = CW_USEDEFAULT;

  auto view = curt::createWindowEx(
    WS_EX_APPWINDOW,
    L"mainview",
    L"Hello world example app",
    WS_OVERLAPPEDWINDOW,
    x, x, x, x,
    HWND_DESKTOP,
    nullptr,
    curt::thisModule(),
    nullptr
  );

  curt::subclassControlBackground(view, RGB(255, 255, 255));
  curt::subclassAppView(view);

  uiglue::declareBindings(view, uiglue::defaultBindingHandlers())
    (Message, "text", "bind: message")
  ;

  return view;
}
