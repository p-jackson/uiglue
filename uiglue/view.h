#ifndef VIEW_H
#define VIEW_H

#include <string>
#include <unordered_map>

#include "view_type.h"
#include "fwd_windows.h"

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

    void runCommand(std::string name, View& view) override {
      auto map = ViewModel::uiglueGetPropertyMap(name);
      if (!map)
        throw std::runtime_error("Property map doesn't exist");

      for (; map->name; ++map)
        if (name == map->name)
          return map->handler(vm, view);

      throw std::runtime_error("Command handler doesn't exist: " + name);
    }

  private:
    ViewModelRefImpl(ViewModelRefImpl&); // = delete;
    ViewModelRefImpl& operator=(ViewModelRefImpl&); // = delete;
  };

  class View {
    HWND m_wnd;
    ViewType m_type;
    std::unique_ptr<ViewModelRef> m_vm;
    std::unordered_map<int, std::string> m_commands;

    static std::exception_ptr s_lastError;

  public:
    View();
    View(std::string className, ViewType type);
    View(View&& o);
    View(const View& o);
    ~View();

    HWND get() const;

    void addCommand(int id, std::string command);

    template<class ViewModel>
    void attachViewModel(ViewModel& vm) {
      m_vm = std::make_unique<ViewModelRefImpl<ViewModel>>(vm);
    }

    void detachViewModel();

    static std::exception_ptr getLastError();
    static LRESULT __stdcall WndProc(HWND, unsigned int, WPARAM, LPARAM);

  private:
    bool onMessage(unsigned int, WPARAM, LPARAM, LRESULT& result);
  };

}

#endif
