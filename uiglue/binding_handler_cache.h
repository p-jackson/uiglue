//===-- BindingHandlerCache declaration -----------------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//

#ifndef UIGLUE_BINDING_HANDLER_CACHE_H
#define UIGLUE_BINDING_HANDLER_CACHE_H

#include "binding_handler.h"

#include <memory>
#include <string>
#include <unordered_map>

namespace uiglue {

class BindingHandlerCache {
  using HandlerPtr = std::shared_ptr<const IBindingHandler>;
  std::unordered_map<std::string, HandlerPtr> m_handlers;

public:
  template <class T>
  void addBindingHandler() {
    addBindingHandler(std::make_shared<BindingHandler<T>>());
  }

  HandlerPtr getBindingHandler(std::string name) const;

private:
  void addBindingHandler(HandlerPtr);
};

} // end namespace uiglue

#endif
