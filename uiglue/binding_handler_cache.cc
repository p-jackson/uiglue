//===-- BindingHandlerCache definition ------------------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//

#include "binding_handler_cache.h"

#include "binding_handler.h"

using HandlerPtr = std::shared_ptr<const uiglue::IBindingHandler>;
using std::string;

namespace uiglue {

HandlerPtr BindingHandlerCache::getBindingHandler(string name) const {
  auto it = m_handlers.find(name);
  if (it == end(m_handlers))
    return {};
  return it->second;
}

void BindingHandlerCache::addBindingHandler(HandlerPtr b) {
  auto name = b->name();
  m_handlers[move(name)] = move(b);
}

} // end namespace uiglue
