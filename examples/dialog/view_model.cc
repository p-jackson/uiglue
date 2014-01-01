//===-- MainViewModel class definition ------------------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//

#include "view_model.h"

using uiglue::Observable;
using std::string;

static string formatPercentage(int percentage) {
  return std::to_string(percentage) + "%";
}

static void adjustPercentages(int changed, Observable<int>& o1, Observable<int>& o2) {
  auto p1 = o1();
  auto p2 = o2();

  auto total = changed + p1 + p2;
  if (total == 100)
    return;

  auto over = total - 100;
  p1 = p1 - over / 2 - over %2;
  p2 = p2 - over / 2;

  if (p1 < 0) {
    p1 = 0;
    p2 = 100 - changed;
  }
  else if (p2 < 0) {
    p2 = 0;
    p1 = 100 - changed;
  }
  else if (p1 > 100) {
    p1 = 100;
    p2 = 0;
  }
  else if (p2 > 100) {
    p2 = 100;
    p1 = 0;
  }

  o1(p1);
  o2(p2);
}

namespace dialogExample {

MainViewModel::MainViewModel()
  : redPercentage{ 34 },
    redText{ std::bind(&MainViewModel::computeRedText, this) },
    greenPercentage{ 33 },
    greenText{ std::bind(&MainViewModel::computeGreenText, this) },
    bluePercentage{ 33 },
    blueText{ std::bind(&MainViewModel::computeBlueText, this) }
{
  redPercentage.subscribe([this](int r) mutable {
    adjustPercentages(r, greenPercentage, bluePercentage);
  });

  greenPercentage.subscribe([this](int r) mutable {
    adjustPercentages(r, redPercentage, bluePercentage);
  });

  bluePercentage.subscribe([this](int r) mutable {
    adjustPercentages(r, redPercentage, greenPercentage);
  });
}

string MainViewModel::computeRedText() {
  return formatPercentage(redPercentage());
}

string MainViewModel::computeGreenText() {
  return formatPercentage(greenPercentage());
}

string MainViewModel::computeBlueText() {
  return formatPercentage(bluePercentage());
}

} // end namespace dialogExample
