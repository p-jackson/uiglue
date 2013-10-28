#include "view_parser.h"

#include "win_util.h"

#include <boost/algorithm/string/trim.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/lexical_cast.hpp>

#pragma warning(push, 3)
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#pragma warning(pop)

using namespace uiglue;
using boost::optional;
using boost::property_tree::ptree;
using std::string;

namespace {

  enum class DeclType {
    Literal,
    Bind,
    Resource,
    File
  };

  DeclType getDeclType(string s) {
    if (s.empty())
      return DeclType::Literal;

    switch (s[0]) {
    case 'b':
    case 'r':
    case 'f':
      break;

    default:
      return DeclType::Literal;
    }

    if (s.find("bind:") == 0)
      return DeclType::Bind;
    else if (s.find("resource:") == 0)
      return DeclType::Resource;
    else if (s.find("file:") == 0)
      return DeclType::File;
    else
      return DeclType::Literal;
  }

  string stripDeclPrefix(DeclType type, string s) {
    switch (type) {
    case DeclType::Literal:
      if (s.find("literal:") == 0)
        return boost::trim_copy(s.substr(8));
      return s;

    case DeclType::Bind:
      return boost::trim_copy(s.substr(5));

    case DeclType::Resource:
      return boost::trim_copy(s.substr(9));

    case DeclType::File:
      return boost::trim_copy(s.substr(5));

    default:
      BOOST_ASSERT(0);
      return {};
    }
  }

  unsigned int getResourceId(string name, filesystem::path resHeader) {
    boost::filesystem::ifstream resources(resHeader);
    auto line = std::string{};
    while (std::getline(resources, line)) {
      auto pos = line.find(name);
      if (pos == line.npos)
        continue;
      auto word = boost::trim_copy(line.substr(pos + name.size()));
      return boost::lexical_cast<unsigned int>(word);
    }
    throw std::runtime_error("Resource id doesn't exist: " + name);
  }

  string getStringDecl(string s, filesystem::path resHeader) {
    switch (getDeclType(s)) {
    case DeclType::Literal:
      return stripDeclPrefix(DeclType::Literal, s);

    case DeclType::Resource:
      {
      auto resourceIdStr = stripDeclPrefix(DeclType::Resource, s);
      auto resourceId = getResourceId(resourceIdStr, resHeader);
      return util::loadString(boost::lexical_cast<unsigned int>(resourceId));
      }

    default:
      throw std::runtime_error("Invalid string declaration: " + s);
    }
  }

  HICON getIconDecl(string s, filesystem::path resHeader) {
    if (getDeclType(s) != DeclType::Resource)
      throw std::runtime_error("Invalid icon declaration: " + s);

    auto resourceIdStr = stripDeclPrefix(DeclType::Resource, s);
    auto resourceId = getResourceId(resourceIdStr, resHeader);
    return LoadIconW(uiglue::util::thisModule(), MAKEINTRESOURCEW(resourceId));
  }

}

namespace uiglue {

  struct ViewParser::Impl {
    boost::property_tree::ptree tree;
    filesystem::path header;
  };

  ViewParser::ViewParser(filesystem::path viewPath, filesystem::path resHeader)
    : pimpl{ std::make_unique<Impl>() }
  {
    resHeader = filesystem::absolute(resHeader);
    if (!filesystem::is_regular_file(resHeader))
      throw std::invalid_argument("Resource header doesn't exist: " + resHeader.string());

    boost::filesystem::ifstream file(viewPath);
    boost::property_tree::read_json(file, pimpl->tree);
    pimpl->header = resHeader;
  }

  ViewParser::ViewParser(ViewParser&& o)
    : pimpl{ std::move(o.pimpl) }
  {
  }

  ViewParser::~ViewParser() = default;

  ViewType ViewParser::getType() const {
    auto type = pimpl->tree.get<string>("type");
    if (type == "AppView")
      return ViewType::App;
    else
      throw std::runtime_error("Invalid view type: " + type);
  }

  optional<string> ViewParser::getText() const {
    auto s = pimpl->tree.get_optional<string>("text");
    if (!s)
      s = pimpl->tree.get_optional<string>("title");
    if (!s)
      return {};

    return getStringDecl(s.get(), pimpl->header);
  }

  optional<HICON> ViewParser::getIcon() const {
    auto s = pimpl->tree.get_optional<string>("icon");
    if (s || getDeclType(s.get()) == DeclType::Resource)
      return getIconDecl(s.get(), pimpl->header);
    return {};
  }

  optional<HICON> ViewParser::getSmallIcon() const {
    auto s = pimpl->tree.get_optional<string>("smallIcon");
    if (s && getDeclType(s.get()) == DeclType::Resource)
      return getIconDecl(s.get(), pimpl->header);
    return {};
  }

  optional<unsigned int> ViewParser::getMenuResourceId() const {
    auto s = pimpl->tree.get_optional<string>("menu.menu");
    if (!s || getDeclType(s.get()) != DeclType::Resource)
      return {};
    
    auto resourceIdStr = stripDeclPrefix(DeclType::Resource, s.get());
    return getResourceId(resourceIdStr, pimpl->header);
  }

  optional<std::vector<std::pair<int, string>>> ViewParser::getMenuCommands() const {
    auto menu = pimpl->tree.get_child_optional("menu.commands");
    if (!menu)
      return {};

    auto commands = std::vector<std::pair<int, string>>{};

    for (auto& v : menu.get()) {
      if (v.first.empty())
        continue;

      auto id = (std::isdigit(v.first[0])) ? boost::lexical_cast<unsigned int>(v.first)
                                           : getResourceId(v.first, pimpl->header);

      commands.emplace_back(id, v.second.data());
    }

    return commands;
  }

  boost::optional<KeyValues> ViewParser::getChildControls() const {
    auto children = pimpl->tree.get_child_optional("children");
    if (!children)
      return {};

    auto result = KeyValues{};

    for (auto& child : children.get()) {
      auto type = child.second.get<string>("type");
      result.emplace_back(child.first, type);
    }

    return result;
  }

  boost::optional<BindingDeclarations> ViewParser::getBindingDeclarations() const {
    auto children = pimpl->tree.get_child_optional("children");
    if (!children)
      return {};

    auto result = BindingDeclarations{};

    for (auto& child : children.get()) {
      auto name = child.first;
      for (auto& binding : child.second) {
        if (binding.first == "type")
          continue;

        result[name].emplace_back(binding.first, binding.second.data());
      }
    }

    return result;
  }

}
