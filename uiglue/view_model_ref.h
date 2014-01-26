//===-- ViewModelRef class declaration ------------------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//

#ifndef UIGLUE_VIEW_MODEL_REF_H
#define UIGLUE_VIEW_MODEL_REF_H

#include "curt/fwd_windows.h"

#include <memory>
#include <string>

namespace uiglue {

struct IUntypedObservable;

struct ViewModelRef {
  virtual ~ViewModelRef() {};
  virtual void runCommand(std::string name, HWND view) = 0;
  virtual std::shared_ptr<IUntypedObservable> getObservable(std::string name) = 0;

  ViewModelRef() = default;
  ViewModelRef(const ViewModelRef&) = delete;
  ViewModelRef& operator=(const ViewModelRef&) = delete;
};

} // end namespace uiglue

#endif
