//===-- SliderViewModel class definition ----------------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//

#include "slider_view_model.h"

using uiglue::Observable;
using std::string;

static string formatPercentage(int percentage) {
  return std::to_string(percentage / 100) + "%";
}

static void adjustPercentages(int changed, Observable<int>& o1, Observable<int>& o2) {
  static bool changing = false;
  if (changing)
    return;

  auto p1 = o1();
  auto p2 = o2();

  auto total = changed + p1 + p2;
  if (total == 10000)
    return;

  auto over = total - 10000;
  p1 = p1 - over / 2 - over %2;
  p2 = p2 - over / 2;

  if (p1 < 0) {
    p1 = 0;
    p2 = 10000 - changed;
  }
  else if (p2 < 0) {
    p2 = 0;
    p1 = 10000 - changed;
  }
  else if (p1 > 10000) {
    p1 = 100;
    p2 = 0;
  }
  else if (p2 > 10000) {
    p2 = 10000;
    p1 = 0;
  }

  changing = true;
  o1(p1);
  o2(p2);
  changing = false;
}

namespace dialogExample {

SliderViewModel::SliderViewModel()
  : redPer10k{ 3334 },
    redText{ std::bind(&SliderViewModel::computeRedText, this) },
    greenPer10k{ 3333 },
    greenText{ std::bind(&SliderViewModel::computeGreenText, this) },
    bluePer10k{ 3333 },
    blueText{ std::bind(&SliderViewModel::computeBlueText, this) },
    rgbTriple{ std::bind(&SliderViewModel::computeTriple, this) }
{
  redPer10k.subscribe([this](int r) mutable {
    adjustPercentages(r, greenPer10k, bluePer10k);
  });

  greenPer10k.subscribe([this](int r) mutable {
    adjustPercentages(r, redPer10k, bluePer10k);
  });

  bluePer10k.subscribe([this](int r) mutable {
    adjustPercentages(r, redPer10k, greenPer10k);
  });
}

string SliderViewModel::computeRedText() {
  return formatPercentage(redPer10k());
}

string SliderViewModel::computeGreenText() {
  return formatPercentage(greenPer10k());
}

string SliderViewModel::computeBlueText() {
  return formatPercentage(bluePer10k());
}

std::tuple<int, int, int> SliderViewModel::computeTriple() {
  return std::make_tuple(redPer10k(), greenPer10k(), bluePer10k());
}

} // end namespace dialogExample
