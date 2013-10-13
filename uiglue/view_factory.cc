#include "view_factory.h"

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

      auto control = CreateWindowExW(exStyle, cls.c_str(), cls.c_str(), style, 0, y, 300, 50, parent, reinterpret_cast<HMENU>(ctrlId), util::thisModule(), nullptr);

      if (!control)
        throw std::runtime_error("Failed to create child control: " + name());

      return control;
    }
  };

  bool isClassRegistered(string name) {
    auto asWide = util::utf8ToWide(name);
    WNDCLASSEXW wcex;
    return GetClassInfoExW(util::thisModule(), asWide.c_str(), &wcex) != 0;
  }

  void registerClass(string name, const ViewParser& parser) {
    auto wideName = util::utf8ToWide(name);

    WNDCLASSEXW wcex = { sizeof(WNDCLASSEXW) };

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
  : m_viewFolder(filesystem::absolute(viewFolder)),
    m_resourceHeader(filesystem::absolute(resourceHeader))
  {
    if (!filesystem::is_directory(m_viewFolder))
      throw std::invalid_argument("viewFolder must be a folder: " + m_viewFolder.string());

    if (!filesystem::is_regular_file(m_resourceHeader))
      throw std::invalid_argument("resourceHeader doesn't exist: " + m_resourceHeader.string());

    registerBuiltinControl<Static>();
    registerBuiltinControl<Edit>();
    registerBuiltinControl<Button>();
    registerBuiltinControl<Checkbox>();
  }

  View ViewFactory::createView(string name) const {
    auto jsonPath = m_viewFolder;
    jsonPath /= name + ".json";
    if (!filesystem::is_regular_file(jsonPath))
      throw std::runtime_error("View description doesn't exist: " + jsonPath.string());

    ViewParser parser(jsonPath, m_resourceHeader);

    const auto viewType = parser.getType();

    const auto clsName = jsonPath.string();
    if (!isClassRegistered(clsName))
      registerClass(clsName, parser);

    auto newView = View{clsName, viewType};

    applyViewDeclaration(newView, parser);

    return newView;
  }

  void ViewFactory::registerControl(std::shared_ptr<const ControlFactory> factory) {
    auto name = factory->name();
    m_controlFactories[name] = std::move(factory);
  }

  template<class Builtin>
  void ViewFactory::registerBuiltinControl() {
    registerControl<ControlFactoryT<Builtin>>();
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

    vector<string> idToName;
    parser.eachChild([&](string name, string type, vector<pair<string, string>> bindings) {
      if (idToName.size() + 1 > std::numeric_limits<int>::max())
        throw std::runtime_error("View has too many children");
      
      auto factory = m_controlFactories.find(type);
      if (factory == end(m_controlFactories))
        throw std::runtime_error("Control factory hasn't been registered: " + type);

      idToName.push_back(name);
      auto control = factory->second->create(view.get(), static_cast<int>(idToName.size()));
      SendMessageW(control, WM_SETFONT, reinterpret_cast<WPARAM>(view.getFont()), true);
    });
  }

}
