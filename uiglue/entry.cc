//===-- uiglue entry point ------------------------------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//

#include "computed.h"
#include "main_view.h"
#include "member_map.h"
#include "resource.h"
#include "observable.h"

#include "curt/curt.h"
#include "curt/util.h"

#include <boost/algorithm/string/case_conv.hpp>
#include <functional>
#include <unordered_map>

using std::string;
using namespace curt;
using namespace uiglue;

// Message handler for about box.
INT_PTR __stdcall aboutProc(HWND dlg, unsigned int msg, WPARAM wParam, LPARAM) {
  switch (msg) {
  case WM_INITDIALOG:
    return 1;

  case WM_COMMAND:
    if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
      endDialog(dlg, LOWORD(wParam));
      return TRUE;
    }
    break;
  }

  return 0;
}

class MainViewModel {

  UIGLUE_BEGIN_MEMBER_MAP(MainViewModel)
    UIGLUE_DECLARE_COMMAND(onExit)
    UIGLUE_DECLARE_COMMAND(onAbout)
    UIGLUE_DECLARE_COMMAND(onModalGreeting)
    UIGLUE_DECLARE_PROPERTY(name)
    UIGLUE_DECLARE_PROPERTY(greeting)
    UIGLUE_DECLARE_PROPERTY(shout)
  UIGLUE_END_MEMBER_MAP()

  MainViewModel()
    : shout{ false },
      greeting{ std::bind(&MainViewModel::calculateGreeting, this) }
  {
  }

private:

  Observable<string> name;
  Observable<bool> shout;
  Computed<string> greeting;

  void onExit(HWND view) {
    destroyWindow(view);
  }

  void onAbout(HWND view) {
    dialogBoxParam(thisModule(), IDD_ABOUTBOX, view, aboutProc, 0);
  }

  void onModalGreeting(HWND view) {
    messageBox(view, greeting(), L"Greeting", MB_OK);
  }

  string calculateGreeting() {
    if (name().empty())
      return {};

    auto greeting = "Greetings " + name();

    if (!shout())
      return greeting;

    boost::to_upper(greeting);
    greeting += "!";
    return greeting;
  }

};


int APIENTRY wWinMain(
  _In_ HINSTANCE hInst,
  _In_opt_ HINSTANCE,
  _In_ LPTSTR,
  _In_ int show
) {
  try {
    auto vm = MainViewModel{};
    auto mainView = makeMainView();

    // uiglue::applyBindings(vm, mainView);

    showWindow(mainView, show);
    updateWindow(mainView);

    auto accelTable = loadAccelerators(hInst, IDC_UIGLUE);

    auto result = pumpMessages(mainView, accelTable);

    return static_cast<int>(result);
  }
  catch (std::exception& e) {
    messageBox(nullptr, e.what(), "Exception", MB_OK | MB_ICONERROR);
    return 0;
  }
  catch (...) {
    messageBox(nullptr, "Unknown exception", "Exception", MB_OK | MB_ICONERROR);
    return 0;
  }
}
