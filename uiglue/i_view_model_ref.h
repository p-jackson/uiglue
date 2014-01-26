//===-- IViewModelRef abstract base class ---------------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//

#ifndef UIGLUE_I_VIEW_MODEL_REF_H
#define UIGLUE_I_VIEW_MODEL_REF_H

#include "curt/fwd_windows.h"

#include <memory>
#include <string>

namespace uiglue {

struct IUntypedObservable;

struct IViewModelRef {
  virtual ~IViewModelRef() {};
  virtual void runCommand(std::string name, HWND view) = 0;
  virtual std::shared_ptr<IUntypedObservable> getObservable(std::string name) = 0;

  IViewModelRef() = default;
  IViewModelRef(const IViewModelRef&) = delete;
  IViewModelRef& operator=(const IViewModelRef&) = delete;
};

} // end namespace uiglue

#endif
