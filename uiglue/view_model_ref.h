//===-- ViewModelRef class declaration ------------------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//

#ifndef UIGLUE_VIEW_MODEL_REF_H
#define UIGLUE_VIEW_MODEL_REF_H

#include "member_map.h"
#include "observable.h"

#include <stdexcept>
#include <string>

namespace uiglue {

class UntypedObservable;

struct ViewModelRef {
  virtual ~ViewModelRef() {};
  virtual void runCommand(std::string name, HWND view) = 0;
  virtual UntypedObservable getObservable(std::string name) = 0;

  ViewModelRef() = default;
  ViewModelRef(const ViewModelRef&) = delete;
  ViewModelRef& operator=(const ViewModelRef&) = delete;
};

template<class ViewModel>
class ViewModelRefImpl : public ViewModelRef {
  ViewModel& vm;

public:
  ViewModelRefImpl(ViewModel& vm_) : vm{ vm_ } {}

  void runCommand(std::string name, HWND view) override {
    getMember(name).handler(vm, view);
  }

  UntypedObservable getObservable(std::string name) override {
    return getMember(name).accessor(vm);
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

}

#endif
