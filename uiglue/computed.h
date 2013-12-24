//===-- Computed class template definition --------------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//

#ifndef UIGLUE_COMPUTED_H
#define UIGLUE_COMPUTED_H

#include "observable.h"

#include <functional>
#include <set>
#include <unordered_map>

namespace uiglue {

template<class T>
class Computed {
  using Self = Computed<T>;
  using ComputeFunction = std::function<T()>;

  ComputeFunction m_compute;
  Observable<T> m_inner;
  bool m_needsUpdated;
  std::unordered_map<int, std::shared_ptr<IUntypedObservable>> m_subscriptions;

public:
  Computed() : Computed(nullptr) {
  }

  Computed(ComputeFunction compute)
    : m_compute{ std::move(compute) },
      m_needsUpdated{ true }
  {
    operator()();
  }

  T operator()() {
    if (m_needsUpdated && m_compute) {
      std::set<std::shared_ptr<IUntypedObservable>> dependencies;
      clearDependencies();

      T newValue;
      {
        DependencyTracker tracker(dependencies);
        newValue = m_compute();
      }

      m_inner(std::move(newValue));

      m_needsUpdated = false;

      auto dependencyUpdated = std::bind(&Computed::dependencyUpdated, this, std::placeholders::_1);

      for (auto& observable : dependencies) {
        auto id = observable->subscribe(dependencyUpdated);
        m_subscriptions[id] = observable;
      }
    }

    return m_inner();
  }

  void subscribe(std::function<void(T)> f) {
    m_inner->subscribe(std::move(f));
  }

  UntypedObservable asUntyped() {
    return m_inner.asUntyped();
  }

private:
  void dependencyUpdated(UntypedObservable) {
    m_needsUpdated = true;
    operator()();
  }

  void clearDependencies() {
    for (auto& subscription : m_subscriptions) {
      auto& observable = subscription.second;
      auto id = subscription.first;
      observable->unsubscribe(id);
    }

    m_subscriptions.clear();
  }
};

} // end namespace uiglue

#endif
