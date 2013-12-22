//===-- Observable class template definition ------------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//

#ifndef UIGLUE_OBSERVABLE_H
#define UIGLUE_OBSERVABLE_H

#include <atomic>
#include <boost/variant.hpp>
#include <functional>
#include <set>
#include <unordered_map>

namespace uiglue {

  class UntypedObservable;

  struct IUntypedObservable {
    virtual ~IUntypedObservable() {}
    virtual const type_info& type() const = 0;
    virtual int subscribe(std::function<void(UntypedObservable)> f) = 0;
    virtual void unsubscribe(int id) = 0;
  };


  class DependencyTracker {
    static DependencyTracker* current;

    std::set<std::shared_ptr<IUntypedObservable>>& m_dependecies;

  public:
    DependencyTracker(std::set<std::shared_ptr<IUntypedObservable>>& dependencies);
    ~DependencyTracker();

    DependencyTracker& operator=(DependencyTracker&) = delete;

    static bool isTracking();
    static void track(std::shared_ptr<IUntypedObservable> observable);
  };

  class TypedObservableSubscriberId {
  protected:
    static std::atomic<int> s_nextId;
  };

  template<class T>
  class TypedObservable : public IUntypedObservable,
                          private TypedObservableSubscriberId,
                          public std::enable_shared_from_this<TypedObservable<T>> {
    friend UntypedObservable;

    T m_value;

    using Callback = boost::variant<
      std::function<void(T)>,
      std::function<void(UntypedObservable)>
    >;

    std::unordered_map<int, Callback> m_subscribers;

    struct NotifySubscriber : public boost::static_visitor<void> {
      TypedObservable& ref;
      NotifySubscriber(TypedObservable& ref_) : ref{ ref_ } {}
      NotifySubscriber& operator=(NotifySubscriber&) = delete;

      void operator()(std::function<void(T)>& f) {
        f(ref.m_value);
      }

      void operator()(std::function<void(UntypedObservable)>& f) {
        auto untyped = UntypedObservable{ ref.shared_from_this() };
        f(untyped);
      }
    };

  public:
    T get() {
      if (DependencyTracker::isTracking())
        DependencyTracker::track(shared_from_this());

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

    const type_info& type() const override {
      return typeid(T);
    }

    std::unique_ptr<TypedObservable<T>> copy() const {
      return std::make_unique<TypedObservable<T>>(m_value);
    }
  };

  template<class T>
  class Observable {
    using Self = Observable<T>;

    std::shared_ptr<TypedObservable<T>> m_inner;

  public:
    Observable()
      : m_inner{ std::make_shared<TypedObservable<T>>() }
    {
    }

    Observable(T value) : Observable() {
      m_inner->set(std::move(value));
    }

    Observable(const Observable& o)
      : m_inner{ o.m_inner->copy() }
    {
    }

    Observable(Observable&& o) {
      using std::swap;
      swap(m_inner, o.m_inner);
    }

    Self& operator=(const Observable& o) {
      m_inner = o.m_inner->copy();
      return *this;
    }

    Self& operator=(Observable&& o) {
      using std::swap;
      swap(m_inner, o.m_inner);
      return *this;
    }

    T operator()() {
      return m_inner->get();
    }

    void operator()(T t) {
      m_inner->set(std::move(t));
    }

    int subscribe(std::function<void(T)> f) {
      return m_inner->subscribe(std::move(f));
    }

    UntypedObservable asUntyped();

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
    template<class T>
    Observable<T> as() {
      if (!is<T>())
        throw std::bad_cast();

      auto asTyped = std::dynamic_pointer_cast<TypedObservable<T>>(m_inner);
      return Observable<T>{ asTyped };
    }

    template<class T>
    bool is() {
      if (DependencyTracker::isTracking())
        DependencyTracker::track(m_inner);

      return m_inner->type() == typeid(T);
    }

    int subscribe(std::function<void(UntypedObservable)> f) {
      return m_inner->subscribe(std::move(f));
    }

    void unsubscribe(int id) {
      m_inner->unsubscribe(id);
    }

  private:
    template<class U> friend class Observable;
    template<class U> friend struct TypedObservable<U>::NotifySubscriber;

    explicit UntypedObservable(std::shared_ptr<IUntypedObservable> i)
      : m_inner{ std::move(i) }
    {
    }
  };

  template<class T>
  UntypedObservable Observable<T>::asUntyped() {
    return UntypedObservable{ m_inner };
  }

}

#endif
