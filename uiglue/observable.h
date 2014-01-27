//===-- Observable class template definition ------------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//
//
// To have a view model property participate in two-way databinding with a view,
// it should be wrapped in an Observable. Observables control access and changes
// to the wrapped value and allow others to subscribe to the changes.
//
// An Observable<T> is default constructible if T is default constructible and
// is copy constructible if T is copy constructible. Observable's are always
// move constructible.
// To be wrapped in an Observable T must be == comparable. The Observable uses
// == in the setter to see if the value really changed before notifying the
// subscribers.
//
//===----------------------------------------------------------------------===//

#ifndef UIGLUE_OBSERVABLE_H
#define UIGLUE_OBSERVABLE_H

#include "i_untyped_observable.h"
#include "make_unique.h"
#include "remove_cv_t.h"
#include "view_model_ref.h"

#include <atomic>
#include <boost/variant.hpp>
#include <functional>
#include <set>
#include <type_traits>
#include <unordered_map>

namespace uiglue {

template<class VM>
struct ViewModelMember;


class DependencyTracker {
  static DependencyTracker* current;

  using DependencySet = std::set<std::shared_ptr<IUntypedObservable>>;

  DependencySet& m_dependecies;

public:
  DependencyTracker(DependencySet& dependencies);
  ~DependencyTracker();

  DependencyTracker& operator=(DependencyTracker&) = delete;

  static bool isTracking();
  static void track(std::shared_ptr<IUntypedObservable> observable);
};

class TypedObservableSubscriberId {
protected:
  static std::atomic<int> s_nextId;
};

namespace detail {

  template<class T, bool isFundamental>
  struct HasMemberMap;

  template<class T>
  struct HasMemberMap<T, true> {
    using type = std::false_type;
  };

  template<class T>
  struct HasMemberMap<T, false> {

    using Result = ViewModelMember<T>*;

    template<class A, A> class Helper {};

    template<class A>
    static std::true_type test(
      A*,
      Helper<Result (*)(), &A::uiglueGetMemberMap>* = nullptr
    );

    static std::false_type test(...);

    using type = decltype(test(static_cast<T*>(nullptr)));
  };

  template<class T>
  struct IsViewModel : HasMemberMap<T, std::is_fundamental<T>::value>::type {};

}

// Provides type-erasure for view model properties. This is lighter-weight than
// an observable because it doesn't do subscription, dependency tracking, and
// it holds on to a reference.
template<class T>
class ValueWrapper : public IUntypedObservable
{
  T& m_value;

public:
  ValueWrapper(T& value) : m_value(value) {}

  T get() {
    return m_value;
  }

  // ValueWrapper ignores subscriptions.
  int subscribe(std::function<void(UntypedObservable)>) override {
    // Won't collide with any ids returned by TypedObservable::subscribe
    return -1;
  }

  void unsubscribe(int) override {}

  const std::type_info& type() const override {
    return typeid(T);
  }

  std::unique_ptr<IViewModelRef> asViewModelRef() override {
    return asViewModelRefInner(detail::IsViewModel<T>{});
  }

  std::unique_ptr<IViewModelRef> asViewModelRefInner(std::true_type) {
    return detail::make_unique<ViewModelRef<T>>(m_value);
  }

  std::unique_ptr<IViewModelRef> asViewModelRefInner(std::false_type) {
    throw std::bad_cast{};
  }
};

template<class T>
class TypedObservable
  : public IUntypedObservable,
    private TypedObservableSubscriberId,
    public std::enable_shared_from_this<TypedObservable<T>>
{
  T m_value;

  using Callback = boost::variant<
    std::function<void(T)>,
    std::function<void(UntypedObservable)>
  >;

  std::unordered_map<int, Callback> m_subscribers;

  struct NotifySubscriber : public boost::static_visitor<void> {
    TypedObservable& m_ref;

    // GCC doesn't accept brace initialised references
    NotifySubscriber(TypedObservable& ref) : m_ref(ref) {}

    NotifySubscriber& operator=(NotifySubscriber&) = delete;

    void operator()(std::function<void(T)>& f) {
      f(m_ref.m_value);
    }

    void operator()(std::function<void(UntypedObservable)>& f) {
      auto untyped = UntypedObservable{ m_ref.shared_from_this() };
      f(untyped);
    }
  };

public:
  template<class... P>
  TypedObservable(P&&... a)
    : m_value{ std::forward<P>(a)... }
  {
  }

  T& get() {
    if (DependencyTracker::isTracking())
      DependencyTracker::track(this->shared_from_this());

    return m_value;
  }

  void set(T t) {
    if (t == m_value)
      return;

    // Notify the subscribers with a copy of the list, because one
    // of them may choose to unsubscribe during the loop, which would
    // invalidate the iterator. Computed observables do this.
    auto subscribersCopy = m_subscribers;

    m_value = std::move(t);
    NotifySubscriber visitor{ *this };
    for (auto& f : subscribersCopy)
      boost::apply_visitor(visitor, f.second);
  }

  int subscribe(std::function<void(UntypedObservable)> f) override {
    auto id = s_nextId++;
    m_subscribers[id] = std::move(f);
    return id;
  }

  void unsubscribe(int id) override {
    m_subscribers.erase(id);
  }

  int subscribe(std::function<void(T)> f) {
    auto id = s_nextId++;
    m_subscribers[id] = std::move(f);
    return id;
  }

  const std::type_info& type() const override {
    return typeid(T);
  }

  std::unique_ptr<IViewModelRef> asViewModelRef() override {
    return asViewModelRefInner(detail::IsViewModel<T>{});
  }

  std::unique_ptr<IViewModelRef> asViewModelRefInner(std::true_type) const {
    return detail::make_unique<ViewModelRef<T>>(m_value);
  }

  std::unique_ptr<IViewModelRef> asViewModelRefInner(std::false_type) const {
    throw std::bad_cast{};
  }

  std::unique_ptr<TypedObservable<T>> copy() const {
    // If the compile fails here then you may be copy constructing an
    // Observable<T> where the type T doesn't have a copy constructor.
    return detail::make_unique<TypedObservable<T>>(m_value);
  }
};


template<class T>
class Observable {
  using Self = Observable<T>;

  std::shared_ptr<TypedObservable<T>> m_inner;

public:
  template<class...>
  struct CanForward : std::true_type {};

  template<class P>
  struct CanForward<P>
    : std::integral_constant<bool, !std::is_same<
        Observable<T>,
        detail::remove_cv_t<P>
      >::value>
  {
  };

  template<
    class... P,
    class = typename std::enable_if<CanForward<P...>::value>::type
  >
  explicit Observable(P&&... a)
    : m_inner { std::make_shared<TypedObservable<T>>(std::forward<P>(a)...) }
  {
  }

  Observable(const Observable& o)
    : m_inner{ o.m_inner->copy() }
  {
    // Bug in VS2013 means this isn't working for =delete'd copy constructors
    // so the compile error will be produced further down the stack.
    // Will catch some cases though.
    static_assert(
      std::is_copy_constructible<T>::value,
      "Copy constructing Observable<T> but T has no copy constructor"
    );
  }

  Observable(Observable&& o)
    : m_inner{ std::move(o.m_inner) }
  {
  }

  Self& operator=(const Observable& o) {
    m_inner = o.m_inner->copy();
    return *this;
  }

  Self& operator=(Observable&& o) {
    m_inner = std::move(o.m_inner);
    return *this;
  }

  T& operator()() {
    return m_inner->get();
  }

  void operator()(T t) {
    m_inner->set(std::move(t));
  }

  int subscribe(std::function<void(T)> f) {
    return m_inner->subscribe(std::move(f));
  }

  void unsubscribe(int subId) {
    m_inner->unsubscribe(subId);
  }

  UntypedObservable asUntyped();

  std::shared_ptr<IUntypedObservable> asUntypedPtr() {
    return m_inner;
  }

private:
  friend UntypedObservable;
  explicit Observable(std::shared_ptr<TypedObservable<T>> i)
    : m_inner{ std::move(i) }
  {
  }
};

class UntypedObservable {
  std::shared_ptr<IUntypedObservable> m_inner;

public:
  explicit UntypedObservable(std::shared_ptr<IUntypedObservable> i)
    : m_inner{ std::move(i) }
  {
  }

  template<class T>
  Observable<T> as() {
    if (!is<T>())
      throw std::bad_cast();

    auto asTyped = std::dynamic_pointer_cast<TypedObservable<T>>(m_inner);
    if (asTyped)
      return Observable<T>{ asTyped };

    auto asValueWrapper = std::dynamic_pointer_cast<ValueWrapper<T>>(m_inner);
    return Observable<T>{ asValueWrapper->get() };
  }

  template<class T>
  bool is() {
    if (DependencyTracker::isTracking())
      DependencyTracker::track(m_inner);

    return m_inner->type() == typeid(T);
  }

  std::unique_ptr<IViewModelRef> asViewModelRef() {
    if (DependencyTracker::isTracking())
      DependencyTracker::track(m_inner);
    return m_inner->asViewModelRef();
  }

  int subscribe(std::function<void(UntypedObservable)> f) {
    return m_inner->subscribe(std::move(f));
  }

  void unsubscribe(int id) {
    m_inner->unsubscribe(id);
  }
};

template<class T>
UntypedObservable Observable<T>::asUntyped() {
  return UntypedObservable{ m_inner };
}

} // end namespace uiglue

#endif
