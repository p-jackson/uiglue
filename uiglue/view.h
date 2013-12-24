//===-- View class declaration --------------------------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//

#ifndef UIGLUE_VIEW_H
#define UIGLUE_VIEW_H

#include "binding_handler_cache.h"
#include "member_map.h"
#include "observable.h"

#include "curt/fwd_windows.h"
#include "curt/types.h"

#include <string>
#include <unordered_map>

namespace uiglue {

  struct ViewModelRef;

  class View {
    using KeyValues = std::vector<std::pair<std::string, std::string>>;
    using BindingDeclarations = std::unordered_map<int, KeyValues>;

    HWND m_wnd;
    std::unique_ptr<ViewModelRef> m_vm;
    std::unordered_map<int, std::string> m_menuCommands;

    KeyValues m_viewBindingDecls;
    BindingDeclarations m_controlBindingDecls;
    BindingHandlerCache m_handlerCache;

    std::unordered_map<WPARAM, std::vector<std::function<void(HWND)>>> m_commandHandlers;
    std::unordered_map<WPARAM, std::vector<std::string>> m_viewModelCommandHandlers;

  public:
    explicit View(HWND wnd);
    ~View();

    void addMenuCommand(int id, std::string command);
    void addCommandHandler(int commandCode, HWND control, std::function<void(HWND)> handler);
    void addCommandHandler(int commandCode, HWND control, std::string viewModelCommand);

    void addBindingHandlerCache(BindingHandlerCache cache);
    void addViewBinding(std::string bindingHandler, std::string bindingText);
    void addControlBinding(int id, std::string bindingHandler, std::string bindingText);

    static LRESULT __stdcall WndProc(HWND, unsigned int, WPARAM, LPARAM, UINT_PTR, DWORD_PTR);

  private:
    bool onMessage(unsigned int, WPARAM, LPARAM, LRESULT& result);
    void applyBindingsToWindow(const KeyValues& bindings, HWND wnd);
    void applyBindings();
    WPARAM getCommandWParam(int commandCode, HWND control);
  };

}

#endif
