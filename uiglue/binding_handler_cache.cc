#include "binding_handler_cache.h"

#include "binding.h"

namespace uiglue {

std::shared_ptr<const Binding> BindingHandlerCache::getBindingHandler(std::string name) const {
  auto it = m_handlers.find(name);
  if (it == end(m_handlers))
    return {};
  return it->second;
}

void BindingHandlerCache::addBindingHandler(std::shared_ptr<const Binding> b) {
  auto name = b->name();
  m_handlers[std::move(name)] = std::move(b);
}

}
