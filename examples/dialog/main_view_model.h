//===-- MainViewModel class declaration -----------------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//

#ifndef DIALOG_MAIN_VIEW_MODEL_H
#define DIALOG_MAIN_VIEW_MODEL_H

#include "slider_view_model.h"

#include "uiglue/member_map.h"
#include "uiglue/observable.h"

#include <string>

namespace dialogExample {

class MainViewModel {
public:
  SliderViewModel slider;

  UIGLUE_BEGIN_MEMBER_MAP(MainViewModel)
    UIGLUE_DECLARE_PROPERTY(slider)
  UIGLUE_END_MEMBER_MAP()

  MainViewModel();
};

} // end namespace dialogExample

#endif
