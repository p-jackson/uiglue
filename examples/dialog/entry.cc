//===-- dialog example entry point ----------------------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//

#include "views.h"
#include "view_model.h"

#include "curt/curt.h"
#include "curt/include_windows.h"
#include "uiglue/bindings.h"

#include <system_error>
#include <sstream>

using namespace curt;
using namespace std;

int APIENTRY wWinMain(
  _In_ HINSTANCE,
  _In_opt_ HINSTANCE,
  _In_ LPTSTR,
  _In_ int show
) {
  Window dialog;

  try {
    dialog = dialogExample::createMainView();
    auto vm = dialogExample::MainViewModel{};

    uiglue::applyBindings(vm, dialog);

    showWindow(dialog, show);

    MSG msg;
    while (getMessage(&msg, nullptr, 0, 0)) {
      if (isDialogMessage(dialog.get(), &msg))
        continue;
      translateMessage(&msg);
      dispatchMessage(&msg);
    }

    return static_cast<int>(msg.wParam);
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
