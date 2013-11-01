#ifndef VIEW_FACTORY_H
#define VIEW_FACTORY_H

#include "binding.h"
#include "fwd.h"
#include "filesystem.h"
#include "view.h"
#include "observable.h"

#include <memory>
#include <unordered_map>

namespace uiglue {

  class ViewParser;

  struct ControlFactory {
    virtual ~ControlFactory() {}
    virtual std::string name() const = 0;

    // If this function succeeds, then the control is owned by the parent and
    // will be destroyed when the parent is destroyed.
    virtual HWND create(HWND parent, int ctrlId) const = 0;
  };

  class ViewFactory {
    filesystem::path m_viewFolder;
    filesystem::path m_resourceHeader;
    std::unordered_map<std::string, std::shared_ptr<const ControlFactory>> m_controlFactories;
    BindingHandlers m_bindingHandlers;

  public:
    ViewFactory(filesystem::path viewFolder, filesystem::path resourceHeader);

    View createView(std::string name) const;

    template<class Factory>
    void registerControl() {
      registerControl(std::make_shared<Factory>());
    }

    template<class T>
    void registerBinding() {
      registerBinding(std::make_shared<T>());
    }

  private:
    void registerControl(std::shared_ptr<const ControlFactory> factory);
    void registerBinding(std::shared_ptr<const Binding> factory);
    void applyViewDeclaration(View& view, const ViewParser& parser) const;
  };

}

#endif
