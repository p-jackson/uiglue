//===-- BindingHandler abstract base class --------------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//

#ifndef UIGLUE_BINDING_HANDLER_H
#define UIGLUE_BINDING_HANDLER_H

#include "curt/fwd_windows.h"

#include <string>

namespace uiglue {

class UntypedObservable;
class View;

struct BindingHandler {
  virtual ~BindingHandler() {}
  BindingHandler() = default;
  BindingHandler(const BindingHandler&) = delete;
  BindingHandler& operator=(const BindingHandler&) = delete;

  virtual std::string name() const = 0;
  virtual void init(HWND ctrl, UntypedObservable ob, View& view) const = 0;
  virtual void update(HWND ctrl, UntypedObservable ob, View& view) const = 0;
};

} // end namespace uiglue

#endif
