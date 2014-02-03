//===-- ValueAccessor class definition ------------------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//

#include "value_accessor.h"

namespace uiglue {

ValueAccessor::ValueAccessor(UntypedObservable observable)
    : m_observable{ std::move(observable) } {
}

bool ValueAccessor::isViewModel() {
  return m_observable.isViewModel();
}

std::unique_ptr<IViewModelRef> ValueAccessor::asViewModelRef() {
  return m_observable.asViewModelRef();
}

} // end namespace uiglue
