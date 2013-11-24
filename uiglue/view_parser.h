//===-- ViewParser class declaration --------------------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//

#ifndef VIEW_PARSER_H
#define VIEW_PARSER_H

#include "filesystem.h"
#include "fwd_windows.h"
#include "view_type.h"

#include <boost/optional.hpp>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

namespace uiglue {

  using KeyValues = std::vector<std::pair<std::string, std::string>>;
  using BindingDeclarations = std::unordered_map<std::string, KeyValues>;

  class ViewParser {
    struct Impl;
    std::unique_ptr<Impl> pimpl;

  public:
    ViewParser(filesystem::path viewPath, filesystem::path resHeader);
    ViewParser(ViewParser&& o);
    ~ViewParser();

    ViewParser& operator=(ViewParser&) = delete;

    ViewType getType() const;
    boost::optional<std::string> getText() const;
    boost::optional<HICON> getIcon() const;
    boost::optional<HICON> getSmallIcon() const;
    boost::optional<unsigned int> getMenuResourceId() const;
    boost::optional<std::vector<std::pair<int, std::string>>> getMenuCommands() const;

    boost::optional<KeyValues> getChildControls() const;
    boost::optional<BindingDeclarations> getBindingDeclarations() const;
  };

}

#endif
