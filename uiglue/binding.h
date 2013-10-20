#ifndef BINDING_H
#define BINDING_H

#include "fwd_windows.h"
#include "observable.h"

#include <memory>
#include <string>
#include <unordered_map>

namespace uiglue {
  
  struct Binding {
    virtual ~Binding() {}

    virtual std::string name() const = 0;
    virtual void init(HWND control, UntypedObservable observable) const = 0;
    virtual void update(HWND control, UntypedObservable observable) const = 0;
  };

  using BindingHandlers = std::unordered_map<std::string, std::shared_ptr<const Binding>>;

}

#endif
