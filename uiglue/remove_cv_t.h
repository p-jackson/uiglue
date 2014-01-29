//===-- remove_cv_t implementation ----------------------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//
//
// remove_cv_t is a C++14 feature which MSVC has already defined but GCC hasn't.
//
//===----------------------------------------------------------------------===//

#ifndef UIGLUE_REMOVE_CV_T_H
#define UIGLUE_REMOVE_CV_T_H

#include <type_traits>

namespace uiglue {
namespace detail {

  template <typename T>
  using remove_cv_t = typename std::remove_cv<T>::type;
}
} // end namespace uiglue::detail

#endif
