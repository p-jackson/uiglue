//===-- IUntypedObservable class declaration ------------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//

#ifndef UIGLUE_UNTYPED_OBSERVABLE_H
#define UIGLUE_UNTYPED_OBSERVABLE_H

#include <functional>
#include <memory>

namespace uiglue {

class UntypedObservable;
struct ViewModelRef;

struct IUntypedObservable {
  virtual ~IUntypedObservable() {}
  virtual const std::type_info& type() const = 0;
  virtual int subscribe(std::function<void(UntypedObservable)> f) = 0;
  virtual void unsubscribe(int id) = 0;
  virtual std::unique_ptr<ViewModelRef> asViewModelRef() = 0;
};

} // end namespace uiglue

#endif
