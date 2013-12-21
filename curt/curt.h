//===-- Win32 function wrappers -------------------------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//
//
// The rationale for these wrappers is to (in order)
// 1) always check error codes and throw on error
// 2) make argument passing more convenient
// 3) favour standard types over Windows typedefs (e.g. BOOL, INT_PTR)
// 
// There are exceptions to point 3 e.g. WPARAM and LPARAM pervasive and leaving
// them in makes it easier to read message handling code.
//
//===----------------------------------------------------------------------===//

#ifndef CURT_CURT_H
#define CURT_CURT_H

#include "api_params.h"
#include "fwd_windows.h"
#include "types.h"

#include <cstdint>

namespace curt {

Font createFontIndirect(const LOGFONTA* logfont);
Font createFontIndirect(const LOGFONTW* logfont);

Window createWindowEx(
  unsigned long exStyle,
  OptString className,
  OptString windowName,
  unsigned long style,
  int x,
  int y,
  int width,
  int height,
  HandleOr<HWND> parent,
  HMENU menu,
  HINSTANCE instance,
  void *createParam
);

LRESULT defSubclassProc(HandleOr<HWND> wnd, unsigned int msg, WPARAM, LPARAM);

void destroyWindow(HandleOr<HWND> wnd);

std::intptr_t dialogBoxParam(
  HINSTANCE hInst,
  StrOrId templateName,
  HandleOr<HWND> parent,
  DLGPROC proc,
  std::intptr_t param
);

LRESULT dispatchMessage(const MSG* msg);

void endDialog(HandleOr<HWND> dlg, std::intptr_t result);

int getDlgCtrlID(HandleOr<HWND> hwndCtl);

HWND getDlgItem(HandleOr<HWND> wnd, int childId);

bool getMessage(
  MSG* msg,
  HandleOr<HWND> wnd,
  unsigned int msgFilterMin,
  unsigned int msgFilterMax
);

HGDIOBJ getStockObject(int object);

HACCEL loadAccelerators(HINSTANCE hInst, StrOrId tableName);

int loadString(
  HINSTANCE hInst,
  unsigned int id,
  wchar_t* buffer,
  int bufferSize
);

int messageBox(
  HandleOr<HWND> parent,
  String text,
  String caption,
  unsigned int type
);

int multiByteToWideChar(
  unsigned int codePage,
  unsigned long flags,
  const char* multiByteStr,
  int multiByteSize,
  wchar_t* wideStr,
  int numChars
);

unsigned int registerWindowMessage(String str);

LRESULT sendDlgItemMessage(
  HandleOr<HWND> dlg,
  int dlgItemId,
  unsigned int msg,
  WPARAM wParam,
  LPARAM lParam
);

LRESULT sendMessage(HandleOr<HWND> wnd, unsigned int, WPARAM, LPARAM);

COLORREF setDCBrushColor(HDC hdc, COLORREF color);

void setWindowSubclass(
  HandleOr<HWND> wnd,
  SUBCLASSPROC subclassProc,
  std::uintptr_t subclassId,
  std::uintptr_t refData
);

void setWindowText(HandleOr<HWND> wnd, String str);

bool showWindow(HandleOr<HWND> wnd, int showCmd);

void systemParametersInfo(
  unsigned int action,
  unsigned int uiParam,
  void* pvParam,
  unsigned int winIni
);

bool translateAccelerator(HandleOr<HWND> wnd, HACCEL accelTable, MSG* msg);

bool translateMessage(const MSG* msg);

void updateWindow(HandleOr<HWND> wnd);

int wideCharToMultiByte(
  unsigned int codePage,
  unsigned long flags,
  const wchar_t* wideStr,
  int numChars,
  char* multiByteStr,
  int multiByteSize,
  const char* defaultChar,
  bool* usedDefaultChar
);

} // end namespace curt

#endif
