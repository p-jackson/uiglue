#ifndef OBSERVABLE_H
#define OBSERVABLE_H

#include <boost/variant.hpp>
#include <functional>
#include <vector>

namespace uiglue {

  class UntypedObservable;

  struct IUntypedObservable {
    virtual ~IUntypedObservable() {}
    virtual const type_info& type() const = 0;
    virtual void subscribe(std::function<void(UntypedObservable)> f) = 0;
  };

  template<class T>
  class TypedObservable : public IUntypedObservable, public std::enable_shared_from_this<TypedObservable<T>> {
    friend UntypedObservable;

    T m_value;

    using Callback = boost::variant<
      std::function<void(T)>,
      std::function<void(UntypedObservable)>
    >;

    std::vector<Callback> m_subscribers;

    struct CallSubscriber : public boost::static_visitor<void> {
      TypedObservable& ref;
      CallSubscriber(TypedObservable& ref_) : ref{ ref_ } {}
      CallSubscriber& operator=(CallSubscriber&) = delete;

      void operator()(std::function<void(T)>& f) {
        f(ref.m_value);
      }

      void operator()(std::function<void(UntypedObservable)>& f) {
        auto untyped = UntypedObservable{ ref.shared_from_this() };
        f(untyped);
      }
    };

  public:
    T get() const {
      return m_value;
    }

    void set(T t) {
      if (t == m_value)
        return;

      m_value = std::move(t);
      CallSubscriber visitor{ *this };
      for (auto& f : m_subscribers)
        boost::apply_visitor(visitor, f);
    }

    void subscribe(std::function<void(UntypedObservable)> f) override {
      m_subscribers.push_back(std::move(f));
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

    void operator()(T t) {
      m_inner->set(std::move(t));
    }

    void subscribe(std::function<void(T)> f) {
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
      if (!is<T>())
        throw std::bad_cast();

      auto asTyped = std::dynamic_pointer_cast<TypedObservable<T>>(m_inner);
      return Observable<T>{ asTyped };
    }

    template<class T>
    bool is() {
      return m_inner->type() == typeid(T);
    }

    void subscribe(std::function<void(UntypedObservable)> f) {
      m_inner->subscribe(std::move(f));
    }

  private:
    template<class U> friend class Observable;
    template<class U> friend struct TypedObservable<U>::CallSubscriber;

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
