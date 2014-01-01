//===-- Main Dialog procedure ---------------------------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//

#include "resource.h"
#include "views.h"

#include "curt/curt.h"
#include "curt/include_windows.h"
#include "curt/util.h"

namespace {

INT_PTR CALLBACK mainViewProc(HWND wnd, UINT Message, WPARAM wParam, LPARAM) {
  switch (Message) {
  case WM_DESTROY:
    PostQuitMessage(0);
    break;
  case WM_INITDIALOG:
    return TRUE;
  case WM_COMMAND:
    switch (LOWORD(wParam)) {
    case IDCANCEL:
      curt::destroyWindow(wnd);
      break;
    }
    break;
  default:
    return FALSE;
  }

  return TRUE;
}

} // end namespace

namespace dialogExample {

curt::Window createMainView() {
  auto inst = curt::thisModule();
  return curt::createDialog(inst, IDD_MAIN_VIEW, HWND_DESKTOP, mainViewProc);
}

} // end namespace dialogExample
