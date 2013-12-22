//===-- Win32 parameter wrapper implementation ----------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//

#include "api_params.h"

#include "include_windows.h"
#include "util.h"

namespace curt {

StrOrId::StrOrId(int i) : str{ MAKEINTRESOURCEW(i) } {}

String::String(const char* s) : str{ curt::utf8ToWide(s) } {}

String::String(const std::string& s) : str { curt::utf8ToWide(s) } {}

} // end namespace curt
