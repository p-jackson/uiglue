//===-- make_unique implementation ----------------------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//
//
// make_unique is a C++14 feature which MSVC has already defined but GCC hasn't.
//
//===----------------------------------------------------------------------===//

#ifndef UIGLUE_MAKE_UNIQUE_H
#define UIGLUE_MAKE_UNIQUE_H

#include <memory>

namespace uiglue { namespace detail {

#if _MSC_VER >= 1800

using std::make_unique;

#else

template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
  return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

#endif

} } // end namespace uiglue::detail

#endif
