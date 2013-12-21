//===-- Win32 parameter wrappers ------------------------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//
//
// These parameter wrappers are used to make the Win32 API functions in curt.h
// more convenient to call. They allow the caller to specify arguments using
// either convenient C++ types or the underlying Win32 types. In either case
// the parameter wrappers convert the arguments to the correct types.
//
//===----------------------------------------------------------------------===//

#ifndef CURT_API_PARAMS_H
#define CURT_API_PARAMS_H

#include "types.h"

#include <string>

namespace curt {

template<typename WindowsHandle>
class HandleOr {
  using HandleType = Handle<detail::HandleTraits<WindowsHandle>>;

  WindowsHandle handle;

public:
  HandleOr(WindowsHandle h) : handle{ h } {}
  HandleOr(const HandleType& h) : handle{ h.get() } {}

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

class OptString : public String {
public:
  OptString(nullptr_t) : String(std::wstring{}) {}
  OptString(const wchar_t* s) : String(s) {}
  OptString(std::wstring s) : String(std::move(s)) {}
  OptString(const char* s) : String(s) {}
  OptString(const std::string& s) : String(std::move(s)) {}
};

}

#endif
