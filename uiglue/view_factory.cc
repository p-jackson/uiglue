#include "view_factory.h"

#include "view.h"
#include "win_util.h"
#include "view_parser.h"

using namespace uiglue;
using std::string;

namespace {

  void applyViewDeclaration(View& view, const ViewParser& parser) {
    auto title = parser.getText();
    if (title)
      util::setWindowText(view.get(), title.get());

    auto menuCommands = parser.getMenuCommands();
    if (menuCommands) {
      for (auto& pair : menuCommands.get())
        view.addCommand(pair.first, pair.second);
    }
  }

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

}
