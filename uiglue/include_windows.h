//===-- Windows.h wrapper -------------------------------------------------===//
//
// The definitions defined in Windows.h can be controlled using pre-processor
// macros. To ensure consistent definitions, uiglue code includes this header
// instead of including Windows.h directly.
//
//===----------------------------------------------------------------------===//

#ifndef INCLUDE_WINDOWS_H
#define INCLUDE_WINDOWS_H

#include <SDKDDKVer.h>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#define OEMRESOURCE
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
