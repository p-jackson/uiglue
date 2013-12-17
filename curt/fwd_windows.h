//===-- Forward declare Windows.h types -----------------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//

#ifndef CURT_FWD_WINDOWS_H
#define CURT_FWD_WINDOWS_H

#if defined(_WIN64)
  typedef __int64 INT_PTR;
  typedef unsigned __int64 UINT_PTR;
  typedef unsigned __int64 WPARAM;
  typedef unsigned __int64 DWORD_PTR;
  typedef __int64 LPARAM;
  typedef __int64 LRESULT;
#else
  typedef _W64 int INT_PTR;
  typedef _W64 unsigned int UINT_PTR;
  typedef _W64 unsigned int WPARAM;
  typedef _W64 unsigned long DWORD_PTR;
  typedef _W64 long LPARAM;
  typedef _W64 long LRESULT;
#endif


struct HINSTANCE__;
typedef struct HINSTANCE__* HINSTANCE;

struct HWND__;
typedef struct HWND__* HWND;

struct HICON__;
typedef struct HICON__* HICON;

struct HFONT__;
typedef struct HFONT__* HFONT;

struct HACCEL__;
typedef struct HACCEL__* HACCEL;


typedef INT_PTR (__stdcall* DLGPROC)(HWND, unsigned int, WPARAM, LPARAM);
typedef LRESULT (__stdcall *SUBCLASSPROC)(HWND, unsigned int, WPARAM, LPARAM, UINT_PTR, DWORD_PTR);

#endif
