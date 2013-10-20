#ifndef COMMANDS_H
#define COMMANDS_H

#include "fwd.h"

#include <string>

#define UIGLUE_BEGIN_COMMAND_MAP(VM) \
public: \
  static uiglue::Command<VM>* uiglueGetPropertyMap(std::string name) { \
    static uiglue::Command<MainViewModel> commandMap[] = {

#define UIGLUE_DECLARE_COMMAND(command) \
      { #command, [] (MainViewModel& vm, uiglue::View& view) { vm.command(view); } },

#define UIGLUE_END_COMMAND_MAP() \
      { nullptr, nullptr } \
    }; \
    return commandMap;\
  }

#define UIGLUE_BEGIN_PROPERTY_MAP(VM)
#define UIGLUE_DECLARE_PROPERTY(property)
#define UIGLUE_END_PROPERTY_MAP()

namespace uiglue {

  template<class VM>
  struct Command {
    using HandlerSig = void (*) (VM&, uiglue::View&);

    const char* name;
    HandlerSig handler;

    Command(const char* name_, HandlerSig handler_) : name(name_), handler(handler_) {}
  };

}

#endif
