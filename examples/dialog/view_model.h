//===-- MainViewModel class declaration -----------------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//

#ifndef DIALOG_VIEW_MODEL_H
#define DIALOG_VIEW_MODEL_H

#include "uiglue/computed.h"
#include "uiglue/member_map.h"
#include "uiglue/observable.h"

#include <string>

namespace dialogExample {

class MainViewModel {
public:
  uiglue::Observable<int> redPercentage;
  uiglue::Computed<std::string> redText;
  uiglue::Observable<int> greenPercentage;
  uiglue::Computed<std::string> greenText;
  uiglue::Observable<int> bluePercentage;
  uiglue::Computed<std::string> blueText;

  UIGLUE_BEGIN_MEMBER_MAP(MainViewModel)
    UIGLUE_DECLARE_PROPERTY(redText)
    UIGLUE_DECLARE_PROPERTY(redPercentage)
    UIGLUE_DECLARE_PROPERTY(greenText)
    UIGLUE_DECLARE_PROPERTY(greenPercentage)
    UIGLUE_DECLARE_PROPERTY(blueText)
    UIGLUE_DECLARE_PROPERTY(bluePercentage)
  UIGLUE_END_MEMBER_MAP()

  MainViewModel();

private:
  std::string computeRedText();
  std::string computeGreenText();
  std::string computeBlueText();
};

} // end namespace dialogExample

#endif
