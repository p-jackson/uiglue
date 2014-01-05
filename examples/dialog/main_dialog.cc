//===-- Main Dialog procedure ---------------------------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//
//
// Creates the main dialog view with createMainView(), using dlgProc() as the
// dialog procedure.
//
// The main view subclasses the placeholder control in the dialog
// with the graphProc() procedure and creates an instance of the Graph class,
// which handles the drawing of the pie graph.
// A custom binding handler (GraphHandler) is defined to bind the view model
// properties to the custom graph control.
//
//===----------------------------------------------------------------------===//

#include "resource.h"
#include "views.h"

#include "curt/com.h"
#include "curt/curt.h"
#include "curt/error.h"
#include "curt/include_windows.h"
#include "curt/util.h"
#include "uiglue/bindings.h"

#include <cmath>
#include <CommCtrl.h>
#include <d2d1.h>
#include <windowsx.h>

using boost::intrusive_ptr;
using curt::getOut;
using namespace D2D1;

namespace {

// Transforms the range [0, 10000] to [0, 2*pi]
float per10kToRadians(int per10k) {
  return per10k * 2 * 3.141f / 10000;
}

// Returns a position on a circle's parameter given the radius, the circle's
// origin and an angle in radians.
// Zero radians points along the negative y axis. Positive angles are clockwise.
D2D1_POINT_2F positionFromAngle(float angle, float r, D2D1_POINT_2F origin) {
  auto x = std::sin(angle) * r + origin.x;
  auto y = -std::cos(angle) * r + origin.y;
  return { x, y };
}

// Direct2D needs to know whether an arc is more or less than pi radians.
D2D1_ARC_SIZE per10kToArcSize(int per10k) {
  return per10k > 5000 ? D2D1_ARC_SIZE_LARGE : D2D1_ARC_SIZE_SMALL;
}


// This class uses Direct2D to draw the pie graph.
// This graph doesn't create a window for itself, it should be given a handle
// to the window it will draw to. The client should call paint() in response to
// WM_PAINT message.
// Objects of type Graph should be destroyed when the window they're drawing to
// is is destroyed.
class Graph {
  HWND m_wnd;
  intrusive_ptr<ID2D1Factory> m_d2dFactory;
  intrusive_ptr<ID2D1DCRenderTarget> m_rt;
  intrusive_ptr<ID2D1SolidColorBrush> m_brush;
  intrusive_ptr<ID2D1PathGeometry> m_redGeometry;
  intrusive_ptr<ID2D1PathGeometry> m_greenGeometry;
  intrusive_ptr<ID2D1PathGeometry> m_blueGeometry;

public:
  Graph(HWND wnd);

  void updateRGB(int r, int g, int b);
  void paint();

private:
  void createDeviceResources();
  void updateGeometry(int r, int g, int b);
};


Graph::Graph(HWND wnd) : m_wnd{ wnd } {
  auto type = D2D1_FACTORY_TYPE_SINGLE_THREADED;
  D2D1CreateFactory(type, __uuidof(ID2D1Factory), getOut(m_d2dFactory));
  updateGeometry(10000, 0, 0);
}

void Graph::updateRGB(int r, int g, int b) {
  updateGeometry(r, g, b);
  // Will causes a WM_PAINT to be sent to the window.
  curt::invalidateRect(m_wnd, nullptr, 0);
}

void Graph::updateGeometry(int red, int green, int blue) {
  RECT rc;
  GetClientRect(m_wnd, &rc);
  const auto width = rc.right - rc.left;
  const auto height = rc.bottom - rc.top;
  const auto radius = 0.9f * std::min(width, height) / 2;
  const auto centre = Point2F(width / 2.f, height / 2.f);

  auto arcSegment = ArcSegment(
    Point2F(),
    SizeF(radius, radius),
    0.f,
    D2D1_SWEEP_DIRECTION_CLOCKWISE,
    D2D1_ARC_SIZE_SMALL
  );

  auto currentPos = Point2F(centre.x, centre.y - radius);

  m_d2dFactory->CreatePathGeometry(getOut(m_redGeometry));
  intrusive_ptr<ID2D1GeometrySink> redSink;
  m_redGeometry->Open(getOut(redSink));
  redSink->SetFillMode(D2D1_FILL_MODE_WINDING);
  redSink->BeginFigure(currentPos, D2D1_FIGURE_BEGIN_FILLED);
  auto angle = per10kToRadians(red);
  arcSegment.point = currentPos = positionFromAngle(angle, radius, centre);
  arcSegment.arcSize = per10kToArcSize(red);
  redSink->AddArc(arcSegment);
  redSink->AddLine(centre);
  redSink->EndFigure(D2D1_FIGURE_END_CLOSED);
  redSink->Close();

  m_d2dFactory->CreatePathGeometry(getOut(m_greenGeometry));
  intrusive_ptr<ID2D1GeometrySink> greenSink;
  m_greenGeometry->Open(getOut(greenSink));
  greenSink->SetFillMode(D2D1_FILL_MODE_WINDING);
  greenSink->BeginFigure(currentPos, D2D1_FIGURE_BEGIN_FILLED);
  auto angleSoFar = per10kToRadians(red + green);
  arcSegment.point = currentPos = positionFromAngle(angleSoFar, radius, centre);
  arcSegment.arcSize = per10kToArcSize(green);
  greenSink->AddArc(arcSegment);
  greenSink->AddLine(centre);
  greenSink->EndFigure(D2D1_FIGURE_END_CLOSED);
  greenSink->Close();

  m_d2dFactory->CreatePathGeometry(getOut(m_blueGeometry));
  intrusive_ptr<ID2D1GeometrySink> blueSink;
  m_blueGeometry->Open(getOut(blueSink));
  blueSink->SetFillMode(D2D1_FILL_MODE_WINDING);
  blueSink->BeginFigure(currentPos, D2D1_FIGURE_BEGIN_FILLED);
  arcSegment.point = Point2F(centre.x, centre.y - radius);
  arcSegment.arcSize = per10kToArcSize(blue);
  blueSink->AddArc(arcSegment);
  blueSink->AddLine(centre);
  blueSink->EndFigure(D2D1_FIGURE_END_CLOSED);
  blueSink->Close();
}

void Graph::createDeviceResources() {
  if (m_rt)
    return;

  const auto props = RenderTargetProperties(
    D2D1_RENDER_TARGET_TYPE_DEFAULT,
    PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE),
    0,
    0,
    D2D1_RENDER_TARGET_USAGE_NONE,
    D2D1_FEATURE_LEVEL_DEFAULT
  );

  m_d2dFactory->CreateDCRenderTarget(&props, getOut(m_rt));
  m_rt->CreateSolidColorBrush(ColorF(0), getOut(m_brush));
}

void Graph::paint() {
  PAINTSTRUCT ps;
  auto dc = curt::beginPaint(m_wnd, &ps);
  RECT rc;
  GetClientRect(m_wnd, &rc);

  createDeviceResources();

  m_rt->BindDC(dc, &rc);
  m_rt->BeginDraw();
  m_rt->SetTransform(Matrix3x2F::Identity());
  m_rt->Clear(ColorF(RGB(240, 240, 240)));

  m_brush->SetColor(ColorF(ColorF::Red));
  m_rt->FillGeometry(m_redGeometry.get(), m_brush.get());
  m_brush->SetColor(ColorF(ColorF::Green));
  m_rt->FillGeometry(m_greenGeometry.get(), m_brush.get());
  m_brush->SetColor(ColorF(ColorF::Blue));
  m_rt->FillGeometry(m_blueGeometry.get(), m_brush.get());

  if (m_rt->EndDraw() == D2DERR_RECREATE_TARGET)
    m_rt.reset();

  ValidateRect(m_wnd, &rc);
}


// Custom message sent by the GraphHandler binding handler and handled by
// the graphProc() procedure.
enum {
  k_graphUpdateTriple = WM_USER + 1
};

// Windows procedure for the custom graph control.
LRESULT CALLBACK graphProc(
  HWND wnd,
  unsigned int msg,
  WPARAM wParam,
  LPARAM lParam,
  UINT_PTR,
  DWORD_PTR data
) {
  try {
    auto graph = reinterpret_cast<Graph*>(data);

    switch (msg) {
    case k_graphUpdateTriple: {
      using std::get;
      auto rgb = *reinterpret_cast<std::tuple<int, int, int>*>(lParam);
      graph->updateRGB(get<0>(rgb), get<1>(rgb), get<2>(rgb));
      break;
    }
    case WM_PAINT:
      graph->paint();
      break;
    case WM_NCDESTROY:
      delete graph;
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


// Custom binding handler to bind rgb triples in a view model to
// a custom graph control. Works for Observable<T>s where T is a
// std::tuple<int, int, int>
struct GraphHandler {
  using UntypedObservable = uiglue::UntypedObservable;
  using View = uiglue::View;

  static std::string name() {
    return { "graph" };
  }

  static void init(HWND wnd, UntypedObservable observable, View& view) {
    GraphHandler::update(wnd, observable, view);
  }

  static void update(HWND wnd, UntypedObservable observable, View&) {
    auto asTuple = observable.as<std::tuple<int, int, int>>();
    auto tuple = asTuple();
    auto asLParam = reinterpret_cast<LPARAM>(&tuple);
    curt::sendMessage(wnd, k_graphUpdateTriple, 0, asLParam);
  }
};


// Create the custom graph control by subclassing the IDC_GRAPH placeholder.
bool onInitDialog(HWND wnd, HWND, LPARAM) {
  using namespace curt;

  auto graphHandle = getDlgItem(wnd, IDC_GRAPH);
  auto graph = std::unique_ptr<Graph>{ new Graph{ graphHandle } };
  auto asUint = reinterpret_cast<std::uintptr_t>(graph.get());
  setWindowSubclass(graphHandle, graphProc, 0, asUint);
  // Passed ownership to the graph control
  graph.release();

  return true;
}

void onCommand(HWND wnd, int id, HWND, UINT) {
  if (id == IDCANCEL)
    curt::destroyWindow(wnd);
}

// Procedure for the main view.
INT_PTR CALLBACK dlgProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam) {
  switch (msg) {
    HANDLE_MSG(wnd, WM_INITDIALOG, onInitDialog);
    HANDLE_MSG(wnd, WM_COMMAND, onCommand);
  default:
    return FALSE;
  }
}

} // end namespace

namespace dialogExample {

// Creates the main dialog view and declares the uiglue bindings.
curt::Window createMainView() {
  auto inst = curt::thisModule();
  auto parent = HWND_DESKTOP;
  auto dlg = curt::createDialog(inst, IDD_MAIN_VIEW, parent, dlgProc);

  curt::subclassAppView(dlg);

  auto handlers = uiglue::defaultBindingHandlers();
  handlers.addBindingHandler<GraphHandler>();

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
