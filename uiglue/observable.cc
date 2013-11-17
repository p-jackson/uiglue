#include "observable.h"

using std::set;
using std::shared_ptr;

namespace uiglue {

  std::atomic<int> TypedObservableSubscriberId::s_nextId;

  DependencyTracker* DependencyTracker::current = nullptr;

  DependencyTracker::DependencyTracker(set<shared_ptr<IUntypedObservable>>& dependencies)
      : m_dependecies{ dependencies }
    {
      current = this;
    }

  DependencyTracker::~DependencyTracker() {
    current = nullptr;
  }

  bool DependencyTracker::isTracking() {
    return !!current;
  }

  void DependencyTracker::track(shared_ptr<IUntypedObservable> observable) {
    if (current)
      current->m_dependecies.insert(observable);
  }

}
