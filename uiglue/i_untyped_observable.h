//===-- IUntypedObservable abstract base class ----------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//

#ifndef UIGLUE_I_UNTYPED_OBSERVABLE_H
#define UIGLUE_I_UNTYPED_OBSERVABLE_H

#include <functional>
#include <memory>

namespace uiglue {

class UntypedObservable;
struct IViewModelRef;

struct IUntypedObservable {
  virtual ~IUntypedObservable() {
  }
  virtual const std::type_info& type() const = 0;
  virtual int subscribe(std::function<void(UntypedObservable)> f) = 0;
  virtual void unsubscribe(int id) = 0;
  virtual std::unique_ptr<IViewModelRef> asViewModelRef() = 0;
  virtual bool isViewModel() = 0;

  IUntypedObservable() = default;
  IUntypedObservable(const IUntypedObservable&) = delete;
  IUntypedObservable& operator=(const IUntypedObservable&) = delete;
};

} // end namespace uiglue

#endif
