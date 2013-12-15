//===-- Wrappers for Win32 API parameters ---------------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//
//
// These wrapper are used to make the Win32 API functions in curt.h more
// convenient to call. They allow the caller to either use convenient C++ types
// or to use the underlying Win32 types. In either case these wrapper's will
// convert the parameters to the correct types.
//
//===----------------------------------------------------------------------===//

#ifndef CURT_API_PARAMS_H
#define CURT_API_PARAMS_H

#include "types.h"

#include <string>

namespace curt {

template<typename WindowsHandle>
class HandleOr {
  using HandleType = Handle<WindowsHandle, detail::HandleTraits>;

  WindowsHandle handle;

public:
  HandleOr(WindowsHandle h) : handle{ h } {}
  HandleOr(const HandleType& h) : handle{ h.get() } {}
  
  HandleOr(const HandleOr&) = delete;
  HandleOr& operator=(const HandleOr&) = delete;

  operator WindowsHandle() const { return handle; }
};

class StrOrId {
  const wchar_t* str;

public:
  StrOrId(const wchar_t* s) : str{ s } {}
  StrOrId(int i);

  StrOrId(const StrOrId&) = delete;
  StrOrId& operator=(const StrOrId&) = delete;

  operator const wchar_t*() const { return str; }
};

class String {
  std::wstring str;

public:
  String(const wchar_t* s) : str{ s } {}
  String(std::wstring s) : str{ std::move(s) } {}
  String(const char* s);
  String(const std::string& s);

  String(const String&) = delete;
  String& operator=(const String) = delete;

  operator const wchar_t*() const { return str.c_str(); }
};

}

#endif
