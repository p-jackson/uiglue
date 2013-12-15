//===-- Error handling helper implementations -----------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//

#include "error.h"

#include "include_windows.h"

#include <exception>
#include <system_error>

static std::exception_ptr savedError;

namespace curt {

void clearCurrentException() {
  savedError = {};
}

void saveCurrentException() {
  savedError = std::current_exception();
}

void throwSavedException() {
  if (savedError != std::exception_ptr())
    std::rethrow_exception(savedError);
}

void throwWin32Error(unsigned long err) {
  auto code = std::error_code{ static_cast<int>(err), std::system_category() };
  throw std::system_error{ code };
}

void throwIfWin32Error() {
  auto err = GetLastError();
  if (err)
    throwWin32Error(err);
}

void throwLastWin32Error() {
  throwWin32Error(GetLastError());
}

} // end namespace curt
