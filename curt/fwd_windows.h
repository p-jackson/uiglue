//===-- Forward declare Windows.h types -----------------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//

#ifndef CURT_FWD_WINDOWS_H
#define CURT_FWD_WINDOWS_H

#include <cstdint>

typedef std::intptr_t INT_PTR;
typedef std::uintptr_t UINT_PTR;
typedef std::uintptr_t WPARAM;

#if defined(_WIN64)
typedef std::uintptr_t DWORD_PTR;
typedef std::intptr_t LPARAM;
typedef std::intptr_t LRESULT;
#else
// Windows.h defines these as longs, not ints
typedef _W64 unsigned long DWORD_PTR;
typedef _W64 long LPARAM;
typedef _W64 long LRESULT;
#endif


#define CURT_DECLARE_HANDLE(name) \
  struct name##__; \
  typedef struct name##__* name

CURT_DECLARE_HANDLE(HINSTANCE);
CURT_DECLARE_HANDLE(HWND);
CURT_DECLARE_HANDLE(HICON);
CURT_DECLARE_HANDLE(HFONT);
CURT_DECLARE_HANDLE(HACCEL);
CURT_DECLARE_HANDLE(HDC);
CURT_DECLARE_HANDLE(HMENU);


#define CURT_DECLARE_STRUCT(name) \
  struct tag##name; \
  typedef struct tag##name name

CURT_DECLARE_STRUCT(MSG);
CURT_DECLARE_STRUCT(LOGFONTW);
CURT_DECLARE_STRUCT(LOGFONTA);
CURT_DECLARE_STRUCT(WNDCLASSEXW);
CURT_DECLARE_STRUCT(WNDCLASSEXA);
CURT_DECLARE_STRUCT(PAINTSTRUCT);
CURT_DECLARE_STRUCT(RECT);


typedef unsigned long COLORREF;
typedef void* HGDIOBJ;
typedef unsigned short ATOM;


typedef INT_PTR (__stdcall* DLGPROC)(HWND, unsigned int, WPARAM, LPARAM);
typedef LRESULT (__stdcall* SUBCLASSPROC)(HWND, unsigned int, WPARAM, LPARAM, UINT_PTR, DWORD_PTR);

#endif
