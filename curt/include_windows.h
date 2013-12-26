//===-- Windows.h wrapper -------------------------------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//
//
// The definitions defined in Windows.h can be controlled using pre-processor
// macros. To ensure consistent definitions, uiglue code includes this header
// instead of including Windows.h directly.
//
//===----------------------------------------------------------------------===//

#ifndef CURT_INCLUDE_WINDOWS_H
#define CURT_INCLUDE_WINDOWS_H

#include <SDKDDKVer.h>

#if !defined(NOMINMAX)
   #define NOMINMAX
#endif

#if !defined(WIN32_LEAN_AND_MEAN)
   #define WIN32_LEAN_AND_MEAN
#endif

#if !defined(OEMRESOURCE)
   #define OEMRESOURCE
#endif

#include <Windows.h>

// If cotire (CMake module) includes Windows.h in it's precompiled header then
// NOMINMAX will have no effect; min/max will already be defined by the
// precompiled header.
// These macros break code so get rid of them.

#if defined(min)
  #undef min
#endif

#if defined(max)
  #undef max
#endif

#endif
