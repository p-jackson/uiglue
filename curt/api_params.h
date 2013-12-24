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

  WindowsHandle m_handle;

public:
  HandleOr(WindowsHandle h) : m_handle{ h } {}
  HandleOr(const HandleType& h) : m_handle{ h.get() } {}

  operator WindowsHandle() const { return m_handle; }
};


class String {
  std::wstring m_str;

public:
  String(const wchar_t* s) : m_str{ s } {}
  String(std::wstring s) : m_str{ std::move(s) } {}
  String(const char* s);
  String(const std::string& s);

  String(const String&) = delete;
  String& operator=(const String) = delete;

  operator const wchar_t*() const { return m_str.c_str(); }
};


class OptString : public String {
  const wchar_t* m_ptr;

  template<class T>
  OptString(bool, T&& t)
    : String(std::forward<T>(t)),
      m_ptr{ String::operator const wchar_t *() }
  {
  }

public:
  OptString(const wchar_t* s) : OptString(true, s) {}
  OptString(std::wstring s) : OptString(true, std::move(s)) {}
  OptString(const char* s) : OptString(true, s) {}
  OptString(const std::string& s) : OptString(true, s) {}

  OptString(nullptr_t)
    : String(std::wstring{}),
      m_ptr{ nullptr }
  {
  }

  operator const wchar_t*() const {
    return m_ptr;
  }
};


class StringOrAtom : public String {
  const wchar_t* m_ptr;

  template<class T>
  StringOrAtom(bool, T&& t)
    : String(std::forward<T>(t)),
      m_ptr{ String::operator const wchar_t *() }
  {
  }

public:
  StringOrAtom(const wchar_t* s) : StringOrAtom(true, s) {}
  StringOrAtom(std::wstring s) : StringOrAtom(true, std::move(s)) {}
  StringOrAtom(const char* s) : StringOrAtom(true, s) {}
  StringOrAtom(const std::string& s) : StringOrAtom(true, s) {}

  StringOrAtom(ATOM atom)
    : String(std::wstring{}),
      m_ptr{ reinterpret_cast<const wchar_t*>(atom) }
  {
  }

  operator const wchar_t*() const {
    return m_ptr;
  }
};


class StringOrId : public String {
  const wchar_t* m_ptr;

  template<class T>
  StringOrId(bool, T&& t)
    : String(std::forward<T>(t)),
      m_ptr{ String::operator const wchar_t *() }
  {
  }

public:
  StringOrId(const wchar_t* s) : StringOrId(true, s) {}
  StringOrId(std::wstring s) : StringOrId(true, std::move(s)) {}
  StringOrId(const char* s) : StringOrId(true, s) {}
  StringOrId(const std::string& s) : StringOrId(true, s) {}

  StringOrId(int id);

  operator const wchar_t*() const {
    return m_ptr;
  }
};

}

#endif
