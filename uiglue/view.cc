//===-- View class definition ---------------------------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//

#include "view.h"

#include "curt/curt.h"
#include "curt/error.h"
#include "curt/include_windows.h"
#include "curt/util.h"

#include <array>
#include <boost/algorithm/string/trim.hpp>

using namespace uiglue;
using std::string;
using std::wstring;

namespace {

  enum class BindType {
    Literal,
    Bind,
    Resource,
    File
  };

  const string k_bind = "bind:";
  const string k_resource = "resource:";
  const string k_file = "file:";
  const string k_literal = "literal:";

  BindType getBindingType(string s) {
    if (s.empty())
      return BindType::Literal;

    if (s.find(k_bind) == 0)
      return BindType::Bind;
    else if (s.find(k_resource) == 0)
      return BindType::Resource;
    else if (s.find(k_file) == 0)
      return BindType::File;
    else
      return BindType::Literal;
  }

  string stripBindingPrefix(BindType type, string s) {
    switch (type) {
    case BindType::Literal:
      if (s.find(k_literal) == 0)
        return boost::trim_copy(s.substr(k_literal.size()));
      return s;

    case BindType::Bind:
      return boost::trim_copy(s.substr(k_bind.size()));

    case BindType::Resource:
      return boost::trim_copy(s.substr(k_resource.size()));

    case BindType::File:
      return boost::trim_copy(s.substr(k_file.size()));

    default:
      BOOST_ASSERT(0);
      return {};
    }
  }

  UntypedObservable makeObservable(std::string binding, ViewModelRef& viewModel) {
    auto type = getBindingType(binding);
    auto value = stripBindingPrefix(type, binding);

    switch (type) {
    case BindType::Literal:
      return Observable<string>{ value }.asUntyped();

    case BindType::Bind:
      return viewModel.getObservable(value);

    default:
      BOOST_ASSERT(0);
      return Observable<string>().asUntyped();
    }
  }

}

namespace uiglue {

  LRESULT __stdcall View::WndProc(HWND wnd, unsigned int msg, WPARAM w, LPARAM l, UINT_PTR, DWORD_PTR refData) {
    try {
      curt::clearCurrentException();

      auto viewPtr = reinterpret_cast<View*>(refData);

      if (msg == WM_NCCREATE)
        viewPtr->m_wnd = wnd;

      auto result = LRESULT{ 0 };
      if (!viewPtr->onMessage(msg, w, l, result))
        result = curt::defSubclassProc(wnd, msg, w, l);

      if (msg == WM_NCDESTROY) {
        viewPtr->m_wnd = nullptr;
        delete viewPtr;
      }

      return result;
    }
    catch (...) {
      curt::saveCurrentException();
      return 0;
    }
  }

  View::View()
    : m_wnd{ nullptr }
  {
  }

  void View::addMenuCommand(int id, std::string command) {
    m_menuCommands[id] = command;
  }

  WPARAM View::getCommandWParam(int commandCode, HWND control) {
    auto id = curt::getDlgCtrlID(control);
    return ((commandCode << 16) | (id & 0xffff));
  }

  void View::addCommandHandler(int commandCode, HWND control, std::function<void(HWND)> handler) {
    auto key = getCommandWParam(commandCode, control);
    m_commandHandlers[key].push_back(std::move(handler));
  }

  void View::addCommandHandler(int commandCode, HWND control, std::string viewModelCommand) {
    auto key = getCommandWParam(commandCode, control);
    m_viewModelCommandHandlers[key].push_back(std::move(viewModelCommand));
  }

  void View::addBindings(BindingDeclarations bindingDeclarations, BindingHandlers bindingHandlers) {
    m_bindingDeclarations = std::move(bindingDeclarations);
    m_bindingHandlers = std::move(bindingHandlers);
  }

  void View::addChildId(int id, std::string name) {
    m_childIds[std::move(name)] = id;
  }

  void View::detachViewModel() {
    m_vm.reset();
  }

  bool View::onMessage(unsigned int msg, WPARAM wParam, LPARAM lParam, LRESULT&) {
    if (msg == WM_COMMAND && HIWORD(wParam) == 0 && !lParam && m_vm) {
      // Handle menu commands
      auto id = LOWORD(wParam);
      auto found = m_menuCommands.find(id);
      if (found != m_menuCommands.end()) {
        m_vm->runCommand(found->second, *this);
        return true;
      }
    }

    if (msg == WM_COMMAND && lParam) {
      // Fire command handlers
      auto handlers = m_commandHandlers.find(wParam);
      if (handlers != end(m_commandHandlers)) {
        for (auto& handler : handlers->second)
          handler(reinterpret_cast<HWND>(lParam));
      }

      auto viewModelCommands = m_viewModelCommandHandlers.find(wParam);
      if (viewModelCommands != end(m_viewModelCommandHandlers)) {
        for (auto& vmCommand : viewModelCommands->second)
          m_vm->runCommand(vmCommand, *this);
      }

      return true;
    }

    return false;
  }

  void View::applyBindings() {
    for (auto& control : m_bindingDeclarations) {
      auto controlHandle = getChildControl(control.first);

      for (auto& binding : control.second) {
        auto handler = m_bindingHandlers.find(binding.first);
        if (handler == end(m_bindingHandlers))
          continue;

        auto observable = makeObservable(binding.second, *m_vm);
        handler->second->init(controlHandle, observable, *this);

        observable.subscribe([handler, controlHandle, this](UntypedObservable o) {
          handler->second->update(controlHandle, o, *this);
        });
      }
    }
  }

  HWND View::getChildControl(std::string name) const {
    auto found = m_childIds.find(name);
    if (found == end(m_childIds))
      throw std::runtime_error("Child control doesn't exist: " + name);

    auto handle = curt::getDlgItem(m_wnd, found->second);

    if (!handle)
      throw std::runtime_error("Child control doesn't exist: " + name);

    return handle;
  }

}
