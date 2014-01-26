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

namespace {

struct With {
  using UntypedObservable = uiglue::UntypedObservable;
  using View = uiglue::View;

  static std::string name() {
    return { "with" };
  }

  static void init(HWND wnd, UntypedObservable observable, View& view) {
    With::update(wnd, observable, view);
  }

  static void update(HWND view, UntypedObservable observable, View&) {
    auto vm = observable.asViewModelRef();
    uiglue::applyBindings(std::move(vm), view);
  }
};


// Create the custom graph control by subclassing the IDC_GRAPH placeholder.
bool onInitDialog(HWND wnd, HWND, LPARAM) {
  auto placeholder = curt::getDlgItem(wnd, IDC_SUB_DIALOG_SIZER);
  RECT rc;
  GetClientRect(placeholder, &rc);
  auto sliderView = dialogExample::createSliderView(wnd);
  curt::setWindowPos(sliderView, nullptr, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER | SWP_SHOWWINDOW);

  SetWindowLongW(sliderView.get(), GWL_ID, IDC_SUB_DIALOG);

  sliderView.release();

  return true;
}

void onCommand(HWND wnd, int id, HWND, UINT) {
  if (id == IDCANCEL)
    curt::destroyWindow(wnd);
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
curt::Window createMainView() {
  auto inst = curt::thisModule();
  auto parent = HWND_DESKTOP;
  auto dlg = curt::createDialog(inst, IDD_MAIN_VIEW, parent, dlgProc);

  curt::subclassAppView(dlg);

  auto handlers = uiglue::defaultBindingHandlers();
  handlers.addBindingHandler<With>();

  uiglue::declareBindings(dlg, handlers)
    (IDC_SUB_DIALOG, "with", "bind: slider")
  ;

  return dlg;
}

} // end namespace dialogExample
