//===-- Mapping view model members ---------------------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//

#ifndef UIGLUE_MEMBER_MAP_H
#define UIGLUE_MEMBER_MAP_H

#include "fwd.h"

#include "curt/fwd_windows.h"

#include <string>

#define UIGLUE_BEGIN_MEMBER_MAP(VM) \
public: \
  static uiglue::ViewModelMember<VM>* uiglueGetMemberMap() { \
    using ThisVM = VM; \
    static uiglue::ViewModelMember<VM> memberMap[] = {

#define UIGLUE_DECLARE_COMMAND(command) \
      { #command, [] (ThisVM& vm, HWND view) { vm.command(view); }, nullptr },

#define UIGLUE_DECLARE_PROPERTY(property) \
      { #property, nullptr, [](ThisVM& vm) { return vm.property.asUntyped(); } },

#define UIGLUE_END_MEMBER_MAP() \
      { nullptr, nullptr, nullptr } \
    }; \
    return memberMap;\
  }

namespace uiglue {

  template<class VM>
  struct ViewModelMember {
    using HandlerSig = void (*) (VM&, HWND);
    using AccessorSig = UntypedObservable (*) (VM&);

    const char* name;
    HandlerSig handler;
    AccessorSig accessor;

    ViewModelMember(const char* name_, HandlerSig handler_, AccessorSig accessor_)
      : name(name_),
        handler(handler_),
        accessor(accessor_)
    {
    }
  };

}

#endif
