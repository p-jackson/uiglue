//===-- Win32 function wrapper implementations ----------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//

#include "curt.h"

#include "error.h"
#include "include_windows.h"
#include "util.h"

#include <CommCtrl.h>

using namespace std;

namespace curt {

Font createFontIndirect(const LOGFONTA* logfont) {
  return { CreateFontIndirectA(logfont) };
}

Font createFontIndirect(const LOGFONTW* logfont) {
  return { CreateFontIndirectW(logfont) };
}

Window createWindowEx(
  unsigned long exStyle,
  StringOrAtom className,
  OptString windowName,
  unsigned long style,
  int x, int y, int w, int h,
  HandleOr<HWND> parent,
  HMENU menu,
  HINSTANCE hInst,
  void *createParam
) {
  auto newWindow = CreateWindowExW(
    exStyle,
    className,
    windowName,
    style,
    x, y, w, h,
    parent,
    menu,
    hInst,
    createParam
  );

  if (!newWindow)
    throwLastWin32Error();

  return { newWindow };
}

void destroyWindow(HandleOr<HWND> wnd) {
  if (!DestroyWindow(wnd))
    throwLastWin32Error();
}

intptr_t dialogBoxParam(
  HINSTANCE hInst,
  StringOrId templateName,
  HandleOr<HWND> parent,
  DLGPROC proc,
  intptr_t param
) {
  auto result = DialogBoxParamW(hInst, templateName, parent, proc, param);
  throwIfWin32Error();
  return result;
}

LRESULT dispatchMessage(const MSG* msg) {
  return DispatchMessageW(msg);
}

void endDialog(HandleOr<HWND> dlg, intptr_t result) {
  if (!EndDialog(dlg, result))
    throwLastWin32Error();
}

int getDlgCtrlID(HandleOr<HWND> hwndCtl) {
  auto result = GetDlgCtrlID(hwndCtl);
  if (!result)
    throwLastWin32Error();
  return result;
}

HWND getDlgItem(HandleOr<HWND> wnd, int childId) {
  auto result = GetDlgItem(wnd, childId);
  if (!result)
    throwLastWin32Error();
  return result;
}

int messageBox(
  HandleOr<HWND> parent,
  String text,
  String caption,
  unsigned int type
) {
  auto result = MessageBoxW(parent, text, caption, type);
  if (!result)
    throwLastWin32Error();
  return result;
}

bool getMessage(
  MSG* msg,
  HandleOr<HWND> wnd,
  unsigned int msgFilterMin,
  unsigned int msgFilterMax
) {
  auto result = GetMessageW(msg, wnd, msgFilterMin, msgFilterMax);
  if (result < 0)
    throwLastWin32Error();
  return result != 0;
}

int multiByteToWideChar(
  unsigned int cp,
  unsigned long flags,
  const char* mbStr,
  int mbSize,
  wchar_t* wideStr,
  int numChars
) {
  auto res = MultiByteToWideChar(cp, flags, mbStr, mbSize, wideStr, numChars);
  if (!res)
    throwLastWin32Error();
  return res;
}

ATOM registerClassEx(const WNDCLASSEXA* wc) {
  auto atom = RegisterClassExA(wc);
  if (!atom)
    throwLastWin32Error();
  return atom;
}

ATOM registerClassEx(const WNDCLASSEXW* wc) {
  auto atom = RegisterClassExW(wc);
  if (!atom)
    throwLastWin32Error();
  return atom;
}

unsigned int registerWindowMessage(String str) {
  auto msg = RegisterWindowMessageW(str);
  if (!msg)
    throwLastWin32Error();
  return msg;
}

LRESULT sendDlgItemMessage(
  HandleOr<HWND> dlg,
  int dlgItemId,
  unsigned int msg,
  WPARAM wParam,
  LPARAM lParam
) {
  return SendDlgItemMessageW(dlg, dlgItemId, msg, wParam, lParam);
}

LRESULT sendMessage(HandleOr<HWND> wnd, unsigned int m, WPARAM w, LPARAM l) {
  auto result = SendMessageW(wnd, m, w, l);
  auto error = GetLastError();
  if (error)
    throwWin32Error(error);
  return result;
}

COLORREF setDCBrushColor(HDC hdc, COLORREF color) {
  auto prev = SetDCBrushColor(hdc, color);
  if (prev == CLR_INVALID)
    throw std::invalid_argument("Invalid color argument for SetDCBrushColor");
  return prev;
}

void setWindowPos(
  HandleOr<HWND> wnd,
  HandleOr<HWND> insertAfter,
  int x, int y, int w, int h,
  unsigned int flags
) {
  if (!SetWindowPos(wnd, insertAfter, x, y, w, h, flags))
    throwLastWin32Error();
}


void setWindowSubclass(
  HandleOr<HWND> wnd,
  SUBCLASSPROC subclassProc,
  std::uintptr_t subclassId,
  std::uintptr_t refData
) {
  if (!SetWindowSubclass(wnd, subclassProc, subclassId, refData))
    throwLastWin32Error();
}

void setWindowText(HandleOr<HWND> wnd, String str) {
  if (!SetWindowTextW(wnd, str))
    throwLastWin32Error();
}

bool showWindow(HandleOr<HWND> wnd, int showCmd) {
  return ShowWindow(wnd, showCmd) != 0;
}

void systemParametersInfo(
  unsigned int action,
  unsigned int uiParam,
  void* pvParam,
  unsigned int winIni
) {
  if (!SystemParametersInfoW(action, uiParam, pvParam, winIni))
    throwLastWin32Error();
}

bool translateAccelerator(HandleOr<HWND> wnd, HACCEL accelTable, MSG* msg) {
  return TranslateAcceleratorW(wnd, accelTable, msg) != 0;
}

bool translateMessage(const MSG* msg) {
  return TranslateMessage(msg) != 0;
}

void updateWindow(HandleOr<HWND> wnd) {
  if (!UpdateWindow(wnd))
    throwLastWin32Error();
}

HGDIOBJ getStockObject(int object) {
  auto result = GetStockObject(object);
  if (!result)
    throwLastWin32Error();
  return result;
}

HACCEL loadAccelerators(HINSTANCE hInst, StringOrId tableName) {
  auto result = LoadAcceleratorsW(hInst, tableName);
  if (!result)
    throwLastWin32Error();
  return result;
}

int loadString(HINSTANCE hInst, unsigned int id, wchar_t* buffer, int buffSz) {
  auto result = LoadStringW(hInst, id, buffer, buffSz);
  if (!result)
    throwLastWin32Error();
  return result;
}

int wideCharToMultiByte(
  unsigned int codePage,
  unsigned long flags,
  const wchar_t* wideStr,
  int numChars,
  char* multiByteStr,
  int multiByteSize,
  const char* defaultChar,
  bool* usedDefaultChar
) {
  auto localUsedDefault = 0;
  if (usedDefaultChar && *usedDefaultChar)
    localUsedDefault = 1;

  auto res = WideCharToMultiByte(
    codePage,
    flags,
    wideStr,
    numChars,
    multiByteStr,
    multiByteSize,
    defaultChar,
    usedDefaultChar ? &localUsedDefault : nullptr
  );

  if (!res)
    throwLastWin32Error();

  if (usedDefaultChar)
    *usedDefaultChar = localUsedDefault != 0;

  return res;
}

} // end namespace curt
