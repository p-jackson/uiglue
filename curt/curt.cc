//===-- Win32 function wrapper implementations ----------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//
//
// The functions in this file try to do as little as possible.
//
// If a Windows function fails, and MSDN says to use GetLastError for more
// information, then call throwLastWin32Error(). This throws a std::system_error
// which wraps up the windows error code and it also looks like it tries to find
// a text description of the error too.
//
// If a Windows function sends a message as part of it's implementation, then
// it should call throwIfSavedException() immediately after it returns. This
// ensures any exceptions thrown in client code are propagated.
//
//===----------------------------------------------------------------------===//

#include "curt.h"

#include "error.h"
#include "include_windows.h"
#include "util.h"

#include <CommCtrl.h>

using namespace std;

namespace curt {

HDC beginPaint(HandleOr<HWND> wnd, PAINTSTRUCT* ps) {
  auto dc = BeginPaint(wnd, ps);
  throwIfSavedException();
  if (!dc)
    throw std::runtime_error("No display context is available");
  return dc;
}

Window createDialog(
  HINSTANCE instance,
  StringOrId templateName,
  HandleOr<HWND> parent,
  DLGPROC proc
) {
  Window dlg = CreateDialogParamW(instance, templateName, parent, proc, 0);
  throwIfSavedException();
  if (!dlg)
    throwLastWin32Error();
  return dlg;
}

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
  Window newWindow = CreateWindowExW(
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

  throwIfSavedException();

  if (!newWindow)
    throwLastWin32Error();

  return newWindow;
}

LRESULT defSubclassProc(HandleOr<HWND> h, unsigned int m, WPARAM w, LPARAM l) {
  auto result = DefSubclassProc(h, m, w, l);
  throwIfSavedException();
  return result;
}

LRESULT defWindowProc(HandleOr<HWND> h, unsigned int m, WPARAM w, LPARAM l) {
  auto result = DefWindowProcW(h, m, w, l);
  throwIfSavedException();
  return result;
}

void destroyWindow(HandleOr<HWND> wnd) {
  auto result = DestroyWindow(wnd);
  throwIfSavedException();
  if (!result)
    throwLastWin32Error();
}

intptr_t dialogBox(
  HINSTANCE hInst,
  StringOrId templateName,
  HandleOr<HWND> parent,
  DLGPROC proc
) {
  auto result = DialogBoxParamW(hInst, templateName, parent, proc, 0);
  throwIfSavedException();
  throwIfWin32Error();
  return result;
}

intptr_t dialogBoxParam(
  HINSTANCE hInst,
  StringOrId templateName,
  HandleOr<HWND> parent,
  DLGPROC proc,
  intptr_t param
) {
  auto result = DialogBoxParamW(hInst, templateName, parent, proc, param);
  throwIfSavedException();
  throwIfWin32Error();
  return result;
}

LRESULT dispatchMessage(const MSG* msg) {
  auto result = DispatchMessageW(msg);
  throwIfSavedException();
  return result;
}

int drawText(HDC dc, String text, int textLen, RECT* rect, unsigned int fmt) {
  auto result = DrawTextW(dc, text, textLen, rect, fmt);
  if (!result)
    throw std::runtime_error("Failed to draw text");
  return result;
}

void endDialog(HandleOr<HWND> dlg, intptr_t result) {
  if (!EndDialog(dlg, result))
    throwLastWin32Error();
}

void endPaint(HandleOr<HWND> wnd, const PAINTSTRUCT* ps) {
  EndPaint(wnd, ps);
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
  throwIfSavedException();
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
  auto result = SendDlgItemMessageW(dlg, dlgItemId, msg, wParam, lParam);
  throwIfSavedException();
  return result;
}

LRESULT sendMessage(HandleOr<HWND> wnd, unsigned int m, WPARAM w, LPARAM l) {
  auto result = SendMessageW(wnd, m, w, l);
  throwIfSavedException();
  if (GetLastError() == 5)
    // Message was blocked by UIPI
    throwLastWin32Error();
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
  auto result = SetWindowPos(wnd, insertAfter, x, y, w, h, flags);
  throwIfSavedException();
  if (!result)
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
  auto result = SetWindowTextW(wnd, str);

  // SetWindowText sends a WM_SETTEXT message
  throwIfSavedException();

  if (!result)
    throwLastWin32Error();
}

bool showWindow(HandleOr<HWND> wnd, int showCmd) {
  auto result = ShowWindow(wnd, showCmd);
  // showWindow is often called outside of a message loop
  throwIfSavedException();
  return result != 0;
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
  auto result = TranslateAcceleratorW(wnd, accelTable, msg) != 0;

  // TranslateAccelerator sends the message directly after translating
  throwIfSavedException();

  return result;
}

bool translateMessage(const MSG* msg) {
  return TranslateMessage(msg) != 0;
}

void updateWindow(HandleOr<HWND> wnd) {
  auto result = UpdateWindow(wnd);
  // updateWindow is often called outside of a message loop
  throwIfSavedException();
  if (!result)
    throwLastWin32Error();
}

HGDIOBJ getStockObject(int object) {
  auto result = GetStockObject(object);
  if (!result)
    throwLastWin32Error();
  return result;
}

int getWindowTextLength(HandleOr<HWND> wnd) {
  auto result = GetWindowTextLengthW(wnd);

  // GetWindowTextLength sends the WM_GETTEXTLENGTH message
  throwIfSavedException();

  throwIfWin32Error();
  return result;
}

template<class CharT, class Func>
int getTextInner(HWND wnd, CharT* buffer, int bufferSize, Func func) {
  auto result = func(wnd, buffer, bufferSize);

  // GetWindowText sends the WM_GETTEXT message
  throwIfSavedException();

  throwIfWin32Error();
  return result;
}

int getWindowText(HandleOr<HWND> wnd, char* buffer, int bufferSize) {
  return getTextInner(wnd, buffer, bufferSize, GetWindowTextA);
}

int getWindowText(HandleOr<HWND> wnd, wchar_t* buffer, int bufferSize) {
  return getTextInner(wnd, buffer, bufferSize, GetWindowTextW);
}

void invalidateRect(HandleOr<HWND> wnd, const RECT* rect, bool erase) {
  auto result = InvalidateRect(wnd, rect, erase ? 1 : 0);
  throwIfSavedException();
  if (!result)
    throw std::runtime_error("Failed to invalidate rect");
}

bool isDialogMessage(HandleOr<HWND> dlg, MSG* msg) {
  auto result = IsDialogMessageW(dlg, msg);
  // IsDialogMessage dispatches messages internally
  throwIfSavedException();
  return result != 0;
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
