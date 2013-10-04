#ifndef VIEW_PARSER_H
#define VIEW_PARSER_H

#include "filesystem.h"
#include "fwd_windows.h"
#include "view_type.h"

#include <boost/optional.hpp>
#include <memory>
#include <string>
#include <vector>

namespace uiglue {

  class ViewParser {
    struct Impl;
    std::unique_ptr<Impl> pimpl;

  public:
    ViewParser(filesystem::path viewPath, filesystem::path resHeader);
    ~ViewParser();

    ViewType getType() const;
    boost::optional<std::string> getText() const;
    boost::optional<HICON> getIcon() const;
    boost::optional<HICON> getSmallIcon() const;
    boost::optional<unsigned int> getMenuResourceId() const;
    boost::optional<std::vector<std::pair<int, std::string>>> getMenuCommands() const;

  };

}

#endif
