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
#include "uiglue/bindings.h"

#include <CommCtrl.h>
#include <windowsx.h>

namespace {

bool onInitDialog(HWND wnd, HWND, LPARAM) {
  using namespace curt;

  auto redSlider = getDlgItem(wnd, IDC_RED_SLIDER);
  auto redText = reinterpret_cast<LPARAM>(getDlgItem(wnd, IDC_RED_TEXT));
  curt::sendMessage(redSlider, TBM_SETBUDDY, 0, redText);

  auto greenSlider = getDlgItem(wnd, IDC_RED_SLIDER);
  auto greenText = reinterpret_cast<LPARAM>(getDlgItem(wnd, IDC_RED_TEXT));
  curt::sendMessage(greenSlider, TBM_SETBUDDY, 0, greenText);

  auto blueSlider = getDlgItem(wnd, IDC_RED_SLIDER);
  auto blueText = reinterpret_cast<LPARAM>(getDlgItem(wnd, IDC_RED_TEXT));
  curt::sendMessage(blueSlider, TBM_SETBUDDY, 0, blueText);

  return true;
}

void onCommand(HWND wnd, int id, HWND, UINT) {
  if (id == IDCANCEL)
    curt::destroyWindow(wnd);
}

INT_PTR CALLBACK proc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam) {
  switch (msg) {
    HANDLE_MSG(wnd, WM_INITDIALOG, onInitDialog);
    HANDLE_MSG(wnd, WM_COMMAND, onCommand);
  default:
    return FALSE;
  }
}

} // end namespace

namespace dialogExample {

curt::Window createMainView() {
  auto inst = curt::thisModule();
  auto parent = HWND_DESKTOP;
  auto dlg = curt::createDialog(inst, IDD_MAIN_VIEW, parent, proc);

  curt::subclassAppView(dlg);

  uiglue::declareBindings(dlg)
    (IDC_RED_TEXT, "text", "bind: redText")
    (IDC_RED_SLIDER, "value", "bind: redPercentage")
    (IDC_RED_SLIDER, "min", 0)
    (IDC_RED_SLIDER, "max", 100)
    (IDC_GREEN_TEXT, "text", "bind: greenText")
    (IDC_GREEN_SLIDER, "value", "bind: greenPercentage")
    (IDC_GREEN_SLIDER, "min", 0)
    (IDC_GREEN_SLIDER, "max", 100)
    (IDC_BLUE_TEXT, "text", "bind: blueText")
    (IDC_BLUE_SLIDER, "value", "bind: bluePercentage")
    (IDC_BLUE_SLIDER, "min", 0)
    (IDC_BLUE_SLIDER, "max", 100)
  ;

  return dlg;
}

} // end namespace dialogExample
