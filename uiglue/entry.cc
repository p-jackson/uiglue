//===-- uiglue entry point ------------------------------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//

#include "resource.h"

#include "computed.h"
#include "main_view.h"
#include "member_map.h"
#include "observable.h"
#include "view.h"
#include "view_factory.h"

#include "curt/include_windows.h"
#include "curt/util.h"

#include <boost/algorithm/string/case_conv.hpp>
#include <functional>
#include <unordered_map>

using uiglue::Computed;
using uiglue::Observable;
using uiglue::View;
using std::string;

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM) {
  switch (message) {
  case WM_INITDIALOG:
    return 1;

  case WM_COMMAND:
    if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
      EndDialog(hDlg, LOWORD(wParam));
      return (INT_PTR)TRUE;
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

  void onExit(View& view) {
    DestroyWindow(view.get());
  }

  void onAbout(View& view) {
    DialogBoxParamW(curt::thisModule(), MAKEINTRESOURCE(IDD_ABOUTBOX), view.get(), About, 0);
  }

  void onModalGreeting(View& view) {
    auto wideGreeting = curt::utf8ToWide(greeting());
    MessageBoxW(view.get(), wideGreeting.c_str(), L"Greeting", MB_OK);
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


int APIENTRY wWinMain(_In_ HINSTANCE hInst, _In_opt_ HINSTANCE, _In_ LPTSTR, _In_ int show) {
  try {
    auto vm = MainViewModel{};
    auto mainView = uiglue::makeMainView();

    //uiglue::attachViewModel(vm, mainView.get());

    ShowWindow(mainView.get(), show);
    UpdateWindow(mainView.get());

    auto accelTable = LoadAcceleratorsW(hInst, MAKEINTRESOURCE(IDC_UIGLUE));

    auto result = curt::pumpMessages(mainView.get(), accelTable);

    return static_cast<int>(result);
  }
  catch (std::exception& e) {
    MessageBoxA(nullptr, e.what(), "Exception", MB_OK | MB_ICONERROR);
    return 0;
  }
  catch (...) {
    MessageBoxA(nullptr, "Unknown exception", "Exception", MB_OK | MB_ICONERROR);
    return 0;
  }
}
