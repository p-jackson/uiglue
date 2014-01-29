//===-- ViewModelRef class declaration ------------------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//

#ifndef UIGLUE_VIEW_MODEL_REF_H
#define UIGLUE_VIEW_MODEL_REF_H

#include "i_untyped_observable.h"
#include "i_view_model_ref.h"

#include "curt/fwd_windows.h"

#include <stdexcept>
#include <string>

namespace uiglue {

template <class>
struct ViewModelMember;

template <class ViewModel>
class ViewModelRef : public IViewModelRef {
  ViewModel& vm;

public:
  // GCC doesn't accept brace initialised references
  ViewModelRef(ViewModel& vm_) : vm(vm_) {
  }

  void runCommand(std::string name, HWND view) override {
    auto handler = getMember(name).handler;

    if (!handler)
      throw std::runtime_error("The member \"" + name
                               + "\" is not a command handler");

    handler(vm, view);
  }

  std::shared_ptr<IUntypedObservable> getObservable(std::string name) override {
    auto accessor = getMember(name).accessor;

    if (!accessor)
      throw std::runtime_error("The member \"" + name + "\" is not a property");

    return accessor(vm);
  }

private:
  ViewModelMember<ViewModel> getMember(std::string name) {
    auto map = ViewModel::uiglueGetMemberMap();
    if (!map)
      throw std::runtime_error("Property map doesn't exist");

    for (auto member = map; member->name; ++member)
      if (name == member->name)
        return *member;

    throw std::runtime_error("Member mapping doesn't exist: " + name);
  }
};

} // end namespace uiglue

#endif
