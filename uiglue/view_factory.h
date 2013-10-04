#ifndef VIEW_FACTORY_H
#define VIEW_FACTORY_H

#include "fwd.h"
#include "filesystem.h"
#include "view.h"

namespace uiglue {

  class ViewFactory {
    filesystem::path m_viewFolder;
    filesystem::path m_resourceHeader;

  public:
    ViewFactory(filesystem::path viewFolder, filesystem::path resourceHeader);

    View createView(std::string name) const;
  };

}

#endif
