#ifndef BINDING_HANDLER_CACHE
#define BINDING_HANDLER_CACHE

#include <memory>
#include <string>
#include <unordered_map>

namespace uiglue {

struct Binding;

class BindingHandlerCache {
  std::unordered_map<std::string, std::shared_ptr<const Binding>> m_handlers;

public:
  template<class T>
  void addBindingHandler() {
    addBindingHandler(std::make_shared<T>());
  }

  std::shared_ptr<const Binding> getBindingHandler(std::string name) const;

private:
  void addBindingHandler(std::shared_ptr<const Binding>);
};

} // end namespace uiglue

#endif
