//===-- Main Dialog procedure ---------------------------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//

#include "resource.h"
#include "views.h"

#include "curt/curt.h"
#include "curt/error.h"
#include "curt/include_windows.h"
#include "curt/util.h"
#include "uiglue/bindings.h"

#include <CommCtrl.h>
#include <d2d1.h>
#include <windowsx.h>

namespace {

enum {
  k_graphInitMsg = WM_USER + 1,
  k_graphSetTriple
};

struct GraphData {
  std::tuple<int, int, int> rgb;
  ID2D1Factory* d2dFactory;
  ID2D1DCRenderTarget* rt;
  ID2D1SolidColorBrush* blackBrush;
  ID2D1SolidColorBrush* redBrush;
  ID2D1SolidColorBrush* greenBrush;
  ID2D1SolidColorBrush* blueBrush;

  GraphData() : rt{ nullptr } {}
};

void onGraphInit(GraphData* data) {
  D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &data->d2dFactory);
}

void createDeviceResources(GraphData* data) {
  if (data->rt)
    return;

  auto props = D2D1::RenderTargetProperties(
    D2D1_RENDER_TARGET_TYPE_DEFAULT,
    D2D1::PixelFormat(
      DXGI_FORMAT_B8G8R8A8_UNORM,
      D2D1_ALPHA_MODE_IGNORE),
    0,
    0,
    D2D1_RENDER_TARGET_USAGE_NONE,
    D2D1_FEATURE_LEVEL_DEFAULT
  );

  data->d2dFactory->CreateDCRenderTarget(&props, &data->rt);

  data->rt->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &data->blackBrush);
  data->rt->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), &data->redBrush);
  data->rt->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Green), &data->greenBrush);
  data->rt->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Blue), &data->blueBrush);
}

void onGraphPaint(HWND wnd, GraphData* data) {
  PAINTSTRUCT ps;
  auto dc = curt::beginPaint(wnd, &ps);
  RECT rc;
  GetClientRect(wnd, &rc);

  auto radius = 0.9 * std::min(rc.right - rc.left, rc.bottom - rc.top) / 2;

  auto centre = D2D1::Point2F((rc.right - rc.left) / 2, (rc.bottom - rc.top) / 2);

  createDeviceResources(data);

  data->rt->BindDC(dc, &rc);
  data->rt->BeginDraw();
  data->rt->SetTransform(D2D1::Matrix3x2F::Identity());
  data->rt->Clear(D2D1::ColorF(RGB(240, 240, 240)));
  
  data->rt->DrawEllipse(D2D1::Ellipse(centre, radius, radius), data->blackBrush);

  data->rt->DrawLine(D2D1::Point2F(centre.x, centre.y - radius), centre, data->redBrush);
  data->rt->DrawLine(D2D1::Point2F(centre.x + radius, centre.y), centre, data->redBrush);
  data->rt->DrawLine(D2D1::Point2F(centre.x, centre.y + radius), centre, data->greenBrush);
  data->rt->DrawLine(D2D1::Point2F(centre.x - radius, centre.y), centre, data->blueBrush);

  if (data->rt->EndDraw() == D2DERR_RECREATE_TARGET)
    data->rt = nullptr;
}

void onGraphDestroy(GraphData* data) {
  auto graphData = reinterpret_cast<GraphData*>(data);
  delete graphData;
}

LRESULT CALLBACK graphProc(HWND wnd, unsigned int msg, WPARAM wParam, LPARAM lParam, UINT_PTR, DWORD_PTR data) {
  try {
    auto graphData = reinterpret_cast<GraphData*>(data);

    switch (msg) {
    case k_graphInitMsg:
      onGraphInit(graphData);
      break;
    case k_graphSetTriple:
      graphData->rgb = *reinterpret_cast<std::tuple<int, int, int>*>(lParam);
      break;
    case WM_PAINT:
      onGraphPaint(wnd, graphData);
      break;
    case WM_DESTROY:
      onGraphDestroy(graphData);
      break;

    default:
      return curt::defSubclassProc(wnd, msg, wParam, lParam);
    }

    return 0;
  }
  catch (...) {
    curt::saveCurrentException();
    return 0;
  }
}

struct Graph {
  static std::string name() {
    return { "graph" };
  }

  static void init(HWND wnd, uiglue::UntypedObservable observable, uiglue::View& view) {
    Graph::update(wnd, observable, view);
  }

  static void update(HWND wnd, uiglue::UntypedObservable observable, uiglue::View&) {
    auto asTriple = observable.as<std::tuple<int, int, int>>();
    auto triple = asTriple();
    curt::sendMessage(wnd, k_graphSetTriple, 0, reinterpret_cast<LPARAM>(&triple));
  }
};

bool onInitDialog(HWND wnd, HWND, LPARAM) {
  using namespace curt;

  auto graph = getDlgItem(wnd, IDC_GRAPH);
  auto graphData = std::unique_ptr<GraphData>{ new GraphData };
  auto asUint = reinterpret_cast<std::uintptr_t>(graphData.get());
  setWindowSubclass(graph, graphProc, 0, asUint);
  // Passed ownership to the graph control
  graphData.release();

  sendMessage(graph, k_graphInitMsg, 0, 0);

  auto redSlider = getDlgItem(wnd, IDC_RED_SLIDER);
  auto redText = reinterpret_cast<LPARAM>(getDlgItem(wnd, IDC_RED_TEXT));
  sendMessage(redSlider, TBM_SETBUDDY, 0, redText);

  auto greenSlider = getDlgItem(wnd, IDC_RED_SLIDER);
  auto greenText = reinterpret_cast<LPARAM>(getDlgItem(wnd, IDC_RED_TEXT));
  sendMessage(greenSlider, TBM_SETBUDDY, 0, greenText);

  auto blueSlider = getDlgItem(wnd, IDC_RED_SLIDER);
  auto blueText = reinterpret_cast<LPARAM>(getDlgItem(wnd, IDC_RED_TEXT));
  sendMessage(blueSlider, TBM_SETBUDDY, 0, blueText);

  return true;
}

void onCommand(HWND wnd, int id, HWND, UINT) {
  if (id == IDCANCEL)
    curt::destroyWindow(wnd);
}

INT_PTR CALLBACK proc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam) {
  switch (msg) {
    HANDLE_MSG(wnd, WM_INITDIALOG, onInitDialog);
    HANDLE_MSG(wnd, WM_COMMAND, onCommand);
  default:
    return FALSE;
  }
}

} // end namespace

namespace dialogExample {

curt::Window createMainView() {
  auto inst = curt::thisModule();
  auto parent = HWND_DESKTOP;
  auto dlg = curt::createDialog(inst, IDD_MAIN_VIEW, parent, proc);

  curt::subclassAppView(dlg);

  auto handlers = uiglue::defaultBindingHandlers();
  handlers.addBindingHandler<Graph>();

  uiglue::declareBindings(dlg, handlers)
    (IDC_RED_SLIDER, "min", 0)
    (IDC_RED_SLIDER, "max", 10000)
    (IDC_RED_SLIDER, "value", "bind: redPer10k")
    (IDC_RED_TEXT, "text", "bind: redText")
    (IDC_GREEN_SLIDER, "min", 0)
    (IDC_GREEN_SLIDER, "max", 10000)
    (IDC_GREEN_SLIDER, "value", "bind: greenPer10k")
    (IDC_GREEN_TEXT, "text", "bind: greenText")
    (IDC_BLUE_SLIDER, "min", 0)
    (IDC_BLUE_SLIDER, "max", 10000)
    (IDC_BLUE_SLIDER, "value", "bind: bluePer10k")
    (IDC_BLUE_TEXT, "text", "bind: blueText")
    (IDC_GRAPH, "graph", "bind: rgbTriple")
  ;

  return dlg;
}

} // end namespace dialogExample
