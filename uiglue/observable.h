#ifndef OBSERVABLE_H
#define OBSERVABLE_H

#include <functional>
#include <vector>

namespace uiglue {

  class UntypedObservable;

  struct IUntypedObservable {
    virtual ~IUntypedObservable() {}
    virtual const type_info& type() const = 0;
  };

  template<class T>
  class TypedObservable : public IUntypedObservable {
    T m_value;
    std::vector<std::function<void(T)>> m_subscribers;

  public:
    T get() const {
      return m_value;
    }

    void set(T t) {
      m_value = std::move(t);
      for (auto& f : m_subscribers)
        f(m_value);
    }

    void subscribe(std::function<void(T)> f) {
      m_subscribers.push_back(std::move(f));
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

    T operator()() const {
      return m_inner->get();
    }

    Self& operator()(T t) {
      m_inner->set(std::move(t));
      return *this;
    }

    Self& subscribe(std::function<void(T)> f) {
      m_inner->subscribe(std::move(f));
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
      if (m_inner->type() != typeid(T))
        throw std::bad_cast();

      auto asTyped = std::dynamic_pointer_cast<TypedObservable<T>>(m_inner);
      return Observable<T>{ asTyped };
    }

  private:
    template<class U> friend class Observable;

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
