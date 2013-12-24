//===-- hello_world entry point -------------------------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//

#include "view.h"

#include "curt/curt.h"
#include "curt/include_windows.h"
#include "curt/util.h"
#include "uiglue/bindings.h"
#include "uiglue/observable.h"

#include <string>

class ViewModel {
public:
  uiglue::Observable<std::string> message;
  uiglue::Observable<std::string> title;

  ViewModel()
    : message{ "Hello, world!" },
      title{ "Hello world example app" }
  {
  }

  UIGLUE_BEGIN_MEMBER_MAP(ViewModel)
    UIGLUE_DECLARE_PROPERTY(message)
    UIGLUE_DECLARE_PROPERTY(title)
  UIGLUE_END_MEMBER_MAP()
};

int APIENTRY wWinMain(
  _In_ HINSTANCE,
  _In_opt_ HINSTANCE,
  _In_ wchar_t*,
  _In_ int show
) {
  try {
    auto view = createView();
    auto vm = ViewModel{};

    uiglue::applyBindings(vm, view);

    curt::showWindow(view, show);
    curt::updateWindow(view);

    auto returnCode = curt::pumpMessages();
    return static_cast<int>(returnCode);
  }
  catch (...) {
    const auto flags = MB_OK | MB_ICONERROR;
    curt::messageBox(nullptr, "Uncaught exception", "Error", flags);
    return 0;
  }
}
