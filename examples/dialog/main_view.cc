//===-- Main dialog procedure ---------------------------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//
//
// Creates the tabbed view with createMainView(), using dlgProc() as the dialog
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

// Memory layout of an extended dialog template
struct DLGTEMPLATEEX {
  WORD dlgVer;
  WORD signature;
  DWORD helpID;
  DWORD exStyle;
  DWORD style;
  WORD cDlgItems;
  short x;
  short y;
  short cx;
  short cy;
  // Rest omitted
};

// Returns the dialog template with the given id.
DLGTEMPLATEEX* lockDialogResource(int id) {
  auto res = findResource(thisModule(), id, RT_DIALOG);
  auto buffer = loadResource(thisModule(), res);

  // LockResource is called that for historical reasons, we don't actually
  // need to unlock anything once we're done.
  return reinterpret_cast<DLGTEMPLATEEX*>(lockResource(buffer));
}

RECT maxSubDialogSize(HWND dlg, std::initializer_list<int> idList) {
  auto max = RECT{};
  for (auto id : idList) {
    auto dlgResource = lockDialogResource(id);
    max.right = std::max<long>(max.right, dlgResource->cx);
    max.bottom = std::max<long>(max.bottom, dlgResource->cy);
  }
  mapDialogRect(dlg, &max);
  return max;
}

POINT duToPix(HWND dlg, POINT pt) {
  auto rc = RECT{ 0, 0, pt.x, pt.y };
  mapDialogRect(dlg, &rc);
  return { rc.right, rc.bottom };
}

bool onInitDialog(HWND wnd, HWND, LPARAM) {
  auto tab = getDlgItem(wnd, IDC_TAB);

  auto item = TCITEMW{ TCIF_TEXT };
  item.pszText = L"Sliders";
  sendMessage(tab, TCM_INSERTITEM, 0, reinterpret_cast<LPARAM>(&item));

  auto rcTab = maxSubDialogSize(wnd, { IDD_SLIDER_VIEW });
  sendMessage(tab, TCM_ADJUSTRECT, true, reinterpret_cast<LPARAM>(&rcTab));

  auto margin = duToPix(wnd, { 7, 7 });
  offsetRect(&rcTab, margin.x - rcTab.left, margin.y - rcTab.top);
  setWindowPos(tab, rcTab);

  auto rcWindow = RECT{ 0, 0, rcTab.right + margin.x, rcTab.bottom + margin.y };
  const auto dlgStyle = GetWindowStyle(wnd);
  const auto dlgExStyle = GetWindowExStyle(wnd);
  adjustWindowRectEx(&rcWindow, dlgStyle, false, dlgExStyle);
  setWindowPos(wnd, rcWindow, SWP_NOMOVE);

  auto display = rcTab;
  sendMessage(tab, TCM_ADJUSTRECT, false, reinterpret_cast<LPARAM>(&display));

  auto sliderView = dialogExample::createSliderView(wnd);
  setWindowLong(sliderView.get(), GWL_ID, IDC_SUB_DIALOG);
  setWindowPos(sliderView, display, SWP_SHOWWINDOW);
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
