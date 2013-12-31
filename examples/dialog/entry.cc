//===-- dialog example entry point ----------------------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//

#include "resource.h"

#include "curt/curt.h"
#include "curt/include_windows.h"

#include <system_error>
#include <sstream>

using namespace curt;
using namespace std;

INT_PTR CALLBACK mainViewProc(HWND wnd, UINT Message, WPARAM wParam, LPARAM) {
  switch (Message) {
  case WM_INITDIALOG:
    return TRUE;
  case WM_COMMAND:
    switch (LOWORD(wParam)) {
    case IDOK:
      EndDialog(wnd, IDOK);
      break;
    case IDCANCEL:
      EndDialog(wnd, IDCANCEL);
      break;
    }
    break;
  default:
    return FALSE;
  }

  return TRUE;
}

int APIENTRY wWinMain(
  _In_ HINSTANCE hInst,
  _In_opt_ HINSTANCE,
  _In_ LPTSTR,
  _In_ int
) {
  try {
    curt::dialogBox(hInst, IDD_MAIN_VIEW, HWND_DESKTOP, mainViewProc);
    return 0;
  }
  catch (system_error& e) {
    ostringstream ss{};
    ss << "System error(" << e.code() << "): " << e.what();
    messageBox(nullptr, ss.str(), "Exception", MB_OK | MB_ICONERROR);
    return 0;
  }
  catch (exception& e) {
    messageBox(nullptr, e.what(), "Exception", MB_OK | MB_ICONERROR);
    return 0;
  }
  catch (...) {
    messageBox(nullptr, "Unknown exception", "Exception", MB_OK | MB_ICONERROR);
    return 0;
  }
}
