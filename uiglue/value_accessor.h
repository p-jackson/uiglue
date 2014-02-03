//===-- ValueAccessor class declaration -----------------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//

#include "observable.h"

#include <memory>

namespace uiglue {

struct IViewModelRef;

class ValueAccessor {
  UntypedObservable m_observable;

public:
  explicit ValueAccessor(UntypedObservable observable);

  template <class T>
  bool is() {
    return m_observable.is<T>();
  }

  template <class T>
  T as() {
    return m_observable.as<T>()();
  }

  template <class T>
  bool isWriteableObservable() {
    return is<T>();
  }

  template <class T>
  Observable<T> asWriteableObservable() {
    return m_observable.as<T>();
  }

  bool isViewModel();

  std::unique_ptr<IViewModelRef> asViewModelRef();
};

} // end namespace uiglue
