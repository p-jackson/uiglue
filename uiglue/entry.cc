#include "resource.h"

#include "commands.h"
#include "observable.h"
#include "view.h"
#include "view_factory.h"
#include "win_util.h"

#include <functional>
#include <unordered_map>

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

  UIGLUE_BEGIN_COMMAND_MAP(MainViewModel)
    UIGLUE_DECLARE_COMMAND(onExit)
    UIGLUE_DECLARE_COMMAND(onAbout)
  UIGLUE_END_COMMAND_MAP()

  UIGLUE_BEGIN_PROPERTY_MAP(MainViewModel)
    UIGLUE_DECLARE_PROPERTY(name)
    UIGLUE_DECLARE_PROPERTY(greeting)
    UIGLUE_DECLARE_PROPERTY(automatic)
  UIGLUE_END_PROPERTY_MAP()

private:

  Observable<string> name;
  Observable<string> greeting;
  Observable<bool> automatic;

  void onExit(View& view) {
    DestroyWindow(view.get());
  }

  void onAbout(View& view) {
    DialogBoxParamW(uiglue::util::thisModule(), MAKEINTRESOURCE(IDD_ABOUTBOX), view.get(), About, 0);
  }

};

int APIENTRY wWinMain(_In_ HINSTANCE hInst, _In_opt_ HINSTANCE, _In_ LPTSTR, _In_ int show) {
  try {
    auto viewFactory = uiglue::ViewFactory{ "../views", "resource.h" };

    auto vm = MainViewModel{};
    auto mainView = viewFactory.createView("main");
    mainView.attachViewModel(vm);

    ShowWindow(mainView.get(), show);
    UpdateWindow(mainView.get());

    auto accelTable = LoadAcceleratorsW(hInst, MAKEINTRESOURCE(IDC_UIGLUE));

    // Main message loop
    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0)) {
      if (!TranslateAcceleratorW(mainView.get(), accelTable, &msg)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
        
        auto error = View::getLastError();
        if (error != std::exception_ptr())
          std::rethrow_exception(error);
      }
    }

    return static_cast<int>(msg.wParam);
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
