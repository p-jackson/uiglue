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

  template<class Traits>
  class ControlFactoryT : public ControlFactory {
  public:
    std::string name() const override {
      return Traits::name();
    }

    HWND create(HWND parent, int ctrlId) const override {
      auto style = Traits::style() | WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS;
      auto exStyle = Traits::exStyle();
      auto cls = Traits::className();

      auto y = (ctrlId - 1) * 50;

      auto control = CreateWindowExW(exStyle, cls.c_str(), nullptr, style, 0, y, 300, 50, parent, reinterpret_cast<HMENU>(ctrlId), util::thisModule(), nullptr);

      if (!control)
        throw std::runtime_error("Failed to create child control: " + name());

      return control;
    }
  };

  template<class Traits>
  class BindingT : public Binding {
  public:
    std::string name() const override {
      return Traits::name();
    }

    void init(HWND wnd, UntypedObservable observable, View& view) const override {
      Traits::init(wnd, std::move(observable), view);
    }

    void update(HWND wnd, UntypedObservable observable, View& view) const override {
      Traits::update(wnd, std::move(observable), view);
    }
  };

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

    registerBuiltinControl<controls::Static>();
    registerBuiltinControl<controls::Edit>();
    registerBuiltinControl<controls::Button>();
    registerBuiltinControl<controls::Checkbox>();

    registerBuiltinBinding<bindings::Text>();
    registerBuiltinBinding<bindings::Title>();
    registerBuiltinBinding<bindings::Value>();
    registerBuiltinBinding<bindings::Visible>();
    registerBuiltinBinding<bindings::Hidden>();
    registerBuiltinBinding<bindings::Checked>();
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

  template<class Builtin>
  void ViewFactory::registerBuiltinControl() {
    registerControl<ControlFactoryT<Builtin>>();
  }

  template<class Builtin>
  void ViewFactory::registerBuiltinBinding() {
    registerBinding<BindingT<Builtin>>();
  }

  void ViewFactory::applyViewDeclaration(View& view, const ViewParser& parser) const {
    auto title = parser.getText();
    if (title)
      util::setWindowText(view.get(), title.get());

    auto menuCommands = parser.getMenuCommands();
    if (menuCommands) {
      for (auto& pair : menuCommands.get())
        view.addCommand(pair.first, pair.second);
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
