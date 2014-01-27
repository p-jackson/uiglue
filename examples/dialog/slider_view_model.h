//===-- SliderViewModel class declaration ---------------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//

#ifndef DIALOG_SLIDER_VIEW_MODEL_H
#define DIALOG_SLIDER_VIEW_MODEL_H

#include "uiglue/computed.h"
#include "uiglue/member_map.h"
#include "uiglue/observable.h"

#include <string>
#include <tuple>

namespace dialogExample {

class SliderViewModel {
public:
  uiglue::Observable<int> redPer10k;
  uiglue::Computed<std::string> redText;
  uiglue::Observable<int> greenPer10k;
  uiglue::Computed<std::string> greenText;
  uiglue::Observable<int> bluePer10k;
  uiglue::Computed<std::string> blueText;
  uiglue::Computed<std::tuple<int, int, int>> rgbTriple;

  UIGLUE_BEGIN_MEMBER_MAP(SliderViewModel)
    UIGLUE_DECLARE_PROPERTY(redText)
    UIGLUE_DECLARE_PROPERTY(redPer10k)
    UIGLUE_DECLARE_PROPERTY(greenText)
    UIGLUE_DECLARE_PROPERTY(greenPer10k)
    UIGLUE_DECLARE_PROPERTY(blueText)
    UIGLUE_DECLARE_PROPERTY(bluePer10k)
    UIGLUE_DECLARE_PROPERTY(rgbTriple)
  UIGLUE_END_MEMBER_MAP()

  SliderViewModel();

private:
  std::string computeRedText();
  std::string computeGreenText();
  std::string computeBlueText();
  std::tuple<int, int, int> computeTriple();
};

} // end namespace dialogExample

#endif
