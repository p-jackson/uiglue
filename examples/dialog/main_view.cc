//===-- Main dialog procedure ---------------------------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//
//
// Creates the slider view with createMainView(), using dlgProc() as the dialog
// procedure.
//
//===----------------------------------------------------------------------===//

#include "resource.h"
#include "views.h"

#include "curt/curt.h"
#include "curt/error.h"
#include "curt/include_windows.h"
#include "curt/util.h"
#include "uiglue/bindings.h"

#include <CommCtrl.h>
#include <windowsx.h>

using namespace curt;

namespace {

// Create the custom graph control by subclassing the IDC_GRAPH placeholder.
bool onInitDialog(HWND wnd, HWND, LPARAM) {
  auto rc = getClientRect(getDlgItem(wnd, IDC_SUB_DIALOG_SIZER));
  auto sliderView = dialogExample::createSliderView(wnd);
  setWindowPos(sliderView, nullptr, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER | SWP_SHOWWINDOW);

  setWindowLong(sliderView.get(), GWL_ID, IDC_SUB_DIALOG);

  sliderView.release();

  return true;
}

void onCommand(HWND wnd, int id, HWND, UINT) {
  if (id == IDCANCEL)
    destroyWindow(wnd);
}

// Procedure for the slider view.
INT_PTR CALLBACK dlgProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam) {
  switch (msg) {
    HANDLE_MSG(wnd, WM_INITDIALOG, onInitDialog);
    HANDLE_MSG(wnd, WM_COMMAND, onCommand);
  default:
    return FALSE;
  }
}

} // end namespace

namespace dialogExample {

// Creates the main view and declares the uiglue bindings.
Window createMainView() {
  auto dlg = createDialog(thisModule(), IDD_MAIN_VIEW, HWND_DESKTOP, dlgProc);

  subclassAppView(dlg);

  uiglue::declareBindings(dlg)
    (IDC_SUB_DIALOG, "with", "bind: slider")
  ;

  return dlg;
}

} // end namespace dialogExample
