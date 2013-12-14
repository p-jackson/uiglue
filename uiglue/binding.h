//===-- Binding abstract base class ---------------------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//

#ifndef BINDING_H
#define BINDING_H

#include "fwd.h"
#include "observable.h"

#include "curt/fwd_windows.h"

#include <memory>
#include <string>
#include <unordered_map>

namespace uiglue {

  struct Binding {
    virtual ~Binding() {}

    virtual std::string name() const = 0;
    virtual void init(HWND control, UntypedObservable observable, View& view) const = 0;
    virtual void update(HWND control, UntypedObservable observable, View& view) const = 0;
  };

  using BindingHandlers = std::unordered_map<std::string, std::shared_ptr<const Binding>>;

}

#endif
