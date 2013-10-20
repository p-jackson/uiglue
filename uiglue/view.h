#ifndef VIEW_H
#define VIEW_H

#include "binding.h"
#include "view_parser.h"
#include "view_type.h"
#include "fwd_windows.h"
#include "win_types.h"

#include <string>
#include <unordered_map>

namespace uiglue {

  class View;

  struct ViewModelRef {
    virtual ~ViewModelRef() {};
    virtual void runCommand(std::string name, View& view) = 0;
  };

  template<class ViewModel>
  class ViewModelRefImpl : public ViewModelRef {
    ViewModel& vm;

  public:
    ViewModelRefImpl(ViewModel& vm_) : vm{vm_} {}

    ViewModelRefImpl(ViewModelRefImpl&) = delete;
    ViewModelRefImpl& operator=(ViewModelRefImpl&) = delete;

    void runCommand(std::string name, View& view) override {
      auto map = ViewModel::uiglueGetPropertyMap(name);
      if (!map)
        throw std::runtime_error("Property map doesn't exist");

      for (; map->name; ++map)
        if (name == map->name)
          return map->handler(vm, view);

      throw std::runtime_error("Command handler doesn't exist: " + name);
    }
  };

  class View {
    HWND m_wnd;
    Font m_font;
    ViewType m_type;
    std::unique_ptr<ViewModelRef> m_vm;
    std::unordered_map<int, std::string> m_commands;
    std::unordered_map<std::string, int> m_childIds;
    BindingDeclarations m_bindingDeclarations;
    BindingHandlers m_bindingHandlers;

    static std::exception_ptr s_lastError;

  public:
    View();
    View(std::string className, ViewType type);
    View(View&& o);
    View(View&) = delete;

    ~View();

    View& operator=(View&& o);
    View& operator=(View&) = delete;

    HWND get() const;

    void addCommand(int id, std::string command);
    void addBindings(BindingDeclarations bindingDeclarations, BindingHandlers bindingHandlers);
    void addChildId(int id, std::string name);
    HFONT getFont() const;

    template<class ViewModel>
    void attachViewModel(ViewModel& vm) {
      m_vm = std::make_unique<ViewModelRefImpl<ViewModel>>(vm);
      applyBindings();
    }

    void detachViewModel();

    static std::exception_ptr getLastError();
    static LRESULT __stdcall WndProc(HWND, unsigned int, WPARAM, LPARAM);

  private:
    bool onMessage(unsigned int, WPARAM, LPARAM, LRESULT& result);
    void applyBindings();
    HWND getChildControl(std::string name) const;
  };

}

#endif
