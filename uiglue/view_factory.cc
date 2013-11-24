//===-- ViewFactory class definition --------------------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//

#include "view_factory.h"

#include "builtin_bindings.h"
#include "builtin_controls.h"
#include "view.h"
#include "win_util.h"
#include "view_parser.h"

#include <boost/algorithm/string/case_conv.hpp>

using namespace uiglue;
using std::string;
using std::vector;
using std::pair;

namespace {

  void registerBuiltinControls(ViewFactory& factory) {
    factory.registerControl<BuiltinControlFactory<controls::Static>>();
    factory.registerControl<BuiltinControlFactory<controls::Edit>>();
    factory.registerControl<BuiltinControlFactory<controls::Button>>();
    factory.registerControl<BuiltinControlFactory<controls::Checkbox>>();
  }

  void registerBuiltinBindings(ViewFactory& factory) {
    factory.registerBinding<BuiltinBinding<bindings::Text>>();
    factory.registerBinding<BuiltinBinding<bindings::Title>>();
    factory.registerBinding<BuiltinBinding<bindings::Value>>();
    factory.registerBinding<BuiltinBinding<bindings::Visible>>();
    factory.registerBinding<BuiltinBinding<bindings::Hidden>>();
    factory.registerBinding<BuiltinBinding<bindings::Checked>>();
    factory.registerBinding<BuiltinBinding<bindings::Click>>();
  }

  bool isClassRegistered(string name) {
    auto asWide = util::utf8ToWide(name);
    WNDCLASSEXW wcex;
    return GetClassInfoExW(util::thisModule(), asWide.c_str(), &wcex) != 0;
  }

  void registerClass(string name, const ViewParser& parser) {
    auto wideName = util::utf8ToWide(name);

    auto wcex = WNDCLASSEXW{ sizeof(WNDCLASSEXW) };

    wcex.lpfnWndProc = &View::WndProc;
    wcex.hInstance = util::thisModule();
    wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wcex.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    wcex.lpszClassName = wideName.c_str();

    auto menuId = parser.getMenuResourceId();
    if (menuId)
      wcex.lpszMenuName = MAKEINTRESOURCEW(menuId.get());

    auto bigIcon = parser.getIcon();
    if (bigIcon)
      wcex.hIcon = bigIcon.get();

    auto smallIcon = parser.getSmallIcon();
    if (smallIcon)
      wcex.hIconSm = smallIcon.get();
    else if (bigIcon)
      wcex.hIconSm = bigIcon.get();

    if (!RegisterClassExW(&wcex))
      throw std::runtime_error("Failed to register window class");
  }

}


namespace uiglue {

  ViewFactory::ViewFactory(filesystem::path viewFolder, filesystem::path resourceHeader)
    : m_viewFolder{ filesystem::absolute(viewFolder) },
      m_resourceHeader{ filesystem::absolute(resourceHeader) }
  {
    if (!filesystem::is_directory(m_viewFolder))
      throw std::invalid_argument("viewFolder must be a folder: " + m_viewFolder.string());

    if (!filesystem::is_regular_file(m_resourceHeader))
      throw std::invalid_argument("resourceHeader doesn't exist: " + m_resourceHeader.string());

    registerBuiltinControls(*this);
    registerBuiltinBindings(*this);

  }

  View ViewFactory::createView(string name) const {
    auto jsonPath = m_viewFolder;
    jsonPath /= name + ".json";
    if (!filesystem::is_regular_file(jsonPath))
      throw std::runtime_error("View description doesn't exist: " + jsonPath.string());

    auto parser = ViewParser{ jsonPath, m_resourceHeader };

    const auto viewType = parser.getType();

    const auto clsName = jsonPath.string();
    if (!isClassRegistered(clsName))
      registerClass(clsName, parser);

    auto newView = View{ clsName, viewType };

    applyViewDeclaration(newView, parser);

    auto bindings = parser.getBindingDeclarations();
    if (bindings)
      newView.addBindings(std::move(bindings.get()), m_bindingHandlers);

    return newView;
  }

  void ViewFactory::registerControl(std::shared_ptr<const ControlFactory> factory) {
    auto name = factory->name();
    m_controlFactories[name] = std::move(factory);
  }

  void ViewFactory::registerBinding(std::shared_ptr<const Binding> binding) {
    auto name = binding->name();
    m_bindingHandlers[name] = std::move(binding);
  }

  void ViewFactory::applyViewDeclaration(View& view, const ViewParser& parser) const {
    auto title = parser.getText();
    if (title)
      util::setWindowText(view.get(), title.get());

    auto menuCommands = parser.getMenuCommands();
    if (menuCommands) {
      for (auto& pair : menuCommands.get())
        view.addMenuCommand(pair.first, pair.second);
    }

    auto children = parser.getChildControls();
    if (children) {
      auto childId = 0;

      for (auto& child : children.get()) {
        auto& name = child.first;
        auto& type = child.second;

        auto factory = m_controlFactories.find(type);
        if (factory == end(m_controlFactories))
          throw std::runtime_error("Control factory hasn't been registered: " + type);

        auto control = factory->second->create(view.get(), ++childId);
        SendMessageW(control, WM_SETFONT, reinterpret_cast<WPARAM>(view.getFont()), true);

        view.addChildId(childId, name);
      }
    }
  }

}
