//===-- View class declaration --------------------------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//

#ifndef VIEW_H
#define VIEW_H

#include "binding.h"
#include "binding_handler_cache.h"
#include "member_map.h"
#include "observable.h"
#include "view_parser.h"
#include "view_model_ref.h"

#include "curt/fwd_windows.h"
#include "curt/types.h"

#include <string>
#include <unordered_map>

namespace uiglue {

  class View {
    using KeyValues = std::vector<std::pair<std::string, std::string>>;
    using BindingDeclarations = std::unordered_map<int, KeyValues>;

    HWND m_wnd;
    std::unique_ptr<ViewModelRef> m_vm;
    std::unordered_map<int, std::string> m_menuCommands;

    BindingDeclarations m_bindingDeclarations;
    BindingHandlerCache m_handlerCache;

    BindingHandlers m_bindingHandlers;
    std::unordered_map<WPARAM, std::vector<std::function<void(HWND)>>> m_commandHandlers;
    std::unordered_map<WPARAM, std::vector<std::string>> m_viewModelCommandHandlers;

  public:
    explicit View(HWND wnd);

    void addMenuCommand(int id, std::string command);
    void addCommandHandler(int commandCode, HWND control, std::function<void(HWND)> handler);
    void addCommandHandler(int commandCode, HWND control, std::string viewModelCommand);
    void addBindings(BindingDeclarations bindingDeclarations, BindingHandlers bindingHandlers);

    void addBindingHandlerCache(BindingHandlerCache cache);
    void addBinding(int id, std::string bindingHandler, std::string bindingText);

    static LRESULT __stdcall WndProc(HWND, unsigned int, WPARAM, LPARAM, UINT_PTR, DWORD_PTR);

  private:
    bool onMessage(unsigned int, WPARAM, LPARAM, LRESULT& result);
    void applyBindings();
    WPARAM getCommandWParam(int commandCode, HWND control);
  };

}

#endif
