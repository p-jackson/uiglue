//===-- View class definition ---------------------------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//

#include "view.h"

#include "binding_handler.h"
#include "i_view_model_ref.h"
#include "view_messages.h"


#include "curt/curt.h"
#include "curt/error.h"
#include "curt/include_windows.h"

#include <boost/algorithm/string/trim.hpp>
#include <CommCtrl.h>

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

struct MakeObservable : public boost::static_visitor<UntypedObservable> {
  IViewModelRef& m_vm;

  // GCC doesn't accept brace initialised references
  MakeObservable(IViewModelRef& vm) : m_vm(vm) {}
  MakeObservable& operator=(const MakeObservable&) = delete;

  UntypedObservable operator()(UntypedObservable value) {
    return value;
  }

  UntypedObservable operator()(string binding) {
    auto type = getBindingType(binding);
    auto value = stripBindingPrefix(type, binding);

    switch (type) {
    case BindType::Literal:
      return Observable<string>{ value }.asUntyped();

    case BindType::Bind:
      return UntypedObservable{ m_vm.getObservable(value) };

    default:
      BOOST_ASSERT(0);
      return Observable<string>().asUntyped();
    }
  }
};

} // end namespace

namespace uiglue {

const char* applyBindingsMsg = "UIGLUE_WM_APPLYBINDINGS";
const char* detachVMMsg = "UIGLUE_WM_DETACHVM";

LRESULT __stdcall View::WndProc(HWND wnd, unsigned int msg, WPARAM w, LPARAM l, UINT_PTR, DWORD_PTR refData) {
  try {
    auto viewPtr = reinterpret_cast<View*>(refData);

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

View::View(HWND wnd)
  : m_wnd{ wnd }
{
}

View::~View() = default;

void View::addMenuCommand(int id, string command) {
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

void View::addCommandHandler(int commandCode, HWND control, string viewModelCommand) {
  auto key = getCommandWParam(commandCode, control);
  m_viewModelCommandHandlers[key].push_back(std::move(viewModelCommand));
}

void View::addMessageHandler(unsigned int msg, std::function<void(WPARAM, LPARAM)> handler) {
  m_msgHandlers[msg].push_back(std::move(handler));
}

void View::addBindingHandlerCache(BindingHandlerCache cache) {
  m_handlerCache = std::move(cache);
}

void View::addViewBinding(string bindingHandler, string bindingText) {
  m_viewBindingDecls.emplace_back(move(bindingHandler), move(bindingText));
}

void View::addViewBinding(string bindingHandler, UntypedObservable value) {
  m_viewBindingDecls.emplace_back(move(bindingHandler), std::move(value));
}

void View::addControlBinding(int id, string bindingHandler, string bindingText) {
  m_controlBindingDecls[id].emplace_back(move(bindingHandler), move(bindingText));
}

void View::addControlBinding(int id, string bindingHandler, UntypedObservable value) {
  m_controlBindingDecls[id].emplace_back(move(bindingHandler), std::move(value));
}

bool View::onMessage(unsigned int msg, WPARAM wParam, LPARAM lParam, LRESULT&) {
  static auto kApplyBindings = curt::registerWindowMessage(applyBindingsMsg);
  static auto kDetachVM = curt::registerWindowMessage(detachVMMsg);

  if (msg == kApplyBindings) {
    m_vm.reset(reinterpret_cast<IViewModelRef*>(lParam));
    applyBindings();
    return true;
  }

  if (msg == kDetachVM) {
    m_vm.reset();
    return true;
  }

  auto msgHandlers = m_msgHandlers.find(msg);
  if (msgHandlers != end(m_msgHandlers)) {
    for (auto& handler : msgHandlers->second)
      handler(wParam, lParam);
  }

  if (msg == WM_COMMAND && HIWORD(wParam) == 0 && !lParam && m_vm) {
    // Handle menu commands
    auto id = LOWORD(wParam);
    auto found = m_menuCommands.find(id);
    if (found != m_menuCommands.end())
      m_vm->runCommand(found->second, m_wnd);
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
        m_vm->runCommand(vmCommand, m_wnd);
    }
  }

  return false;
}

void View::applyBindingsToWindow(const KeyValues& bindings, HWND wnd) {
  for (auto& binding : bindings) {
    auto handler = m_handlerCache.getBindingHandler(binding.first);
    if (!handler)
      continue;

    MakeObservable makeObservable(*m_vm);

    auto observable = boost::apply_visitor(makeObservable, binding.second);
    handler->init(wnd, observable, *this);

    observable.subscribe([handler, wnd, this](UntypedObservable o) {
      handler->update(wnd, o, *this);
    });
  }
}

void View::applyBindings() {
  applyBindingsToWindow(m_viewBindingDecls, m_wnd);

  for (auto& control : m_controlBindingDecls) {
    auto controlHandle = curt::getDlgItem(m_wnd, control.first);
    applyBindingsToWindow(control.second, controlHandle);
  }
}

} // end namespace uiglue
