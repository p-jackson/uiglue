//===-- BindingHandlerCache declaration -----------------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//

#ifndef BINDING_HANDLER_CACHE
#define BINDING_HANDLER_CACHE

#include <memory>
#include <string>
#include <unordered_map>

namespace uiglue {

struct BindingHandler;

class BindingHandlerCache {
  using HandlerPtr = std::shared_ptr<const BindingHandler>;
  std::unordered_map<std::string, HandlerPtr> m_handlers;

public:
  template<class T>
  void addBindingHandler() {
    addBindingHandler(std::make_shared<T>());
  }

  HandlerPtr getBindingHandler(std::string name) const;

private:
  void addBindingHandler(HandlerPtr);
};

} // end namespace uiglue

#endif
