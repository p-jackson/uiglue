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

using namespace std;

namespace curt {

void destroyWindow(HandleOr<HWND> wnd) {
  if (!DestroyWindow(wnd))
    throwLastWin32Error();
}

intptr_t dialogBoxParam(HINSTANCE hInst, StrOrId templateName, HandleOr<HWND> parent, DLGPROC proc, intptr_t param) {
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

int messageBox(HandleOr<HWND> parent, String text, String caption, unsigned int type) {
  auto result = MessageBoxW(parent, text, caption, type);
  if (!result)
    throwLastWin32Error();
  return result;
}

bool getMessage(MSG* msg, HandleOr<HWND> wnd, unsigned int msgFilterMin, unsigned int msgFilterMax) {
  auto result = GetMessageW(msg, wnd, msgFilterMin, msgFilterMax);
  if (result < 0)
    throwLastWin32Error();
  return result != 0;
}

int multiByteToWideChar(unsigned int codePage, unsigned long flags, const char* multiByteStr, int multiByteSize, wchar_t* wideStr, int numChars) {
  auto result = MultiByteToWideChar(codePage, flags, multiByteStr, multiByteSize, wideStr, numChars);
  if (!result)
    throwLastWin32Error();
  return result;
}

void setWindowText(HandleOr<HWND> wnd, String str) {
  if (!SetWindowTextW(wnd, str))
    throwLastWin32Error();
}

bool showWindow(HandleOr<HWND> wnd, int showCmd) {
  return ShowWindow(wnd, showCmd) != 0;
}

bool translateMessage(const MSG* msg) {
  return TranslateMessage(msg) != 0;
}

void updateWindow(HandleOr<HWND> wnd) {
  if (!UpdateWindow(wnd))
    throwLastWin32Error();
}

HACCEL loadAccelerators(HINSTANCE hInst, StrOrId tableName) {
  auto result = LoadAcceleratorsW(hInst, tableName);
  if (!result)
    throwLastWin32Error();
  return result;
}

int loadString(HINSTANCE hInst, unsigned int id, wchar_t* buffer, int bufferSize) {
  auto result = LoadStringW(hInst, id, buffer, bufferSize);
  if (!result)
    throwLastWin32Error();
  return result;
}

int wideCharToMultiByte(unsigned int codePage, unsigned long flags, const wchar_t* wideStr, int numChars, char* multiByteStr, int multiByteSize, const char* defaultChar, bool* usedDefaultChar) {
  auto localUsedDefault = 0;
  if (usedDefaultChar && *usedDefaultChar)
    localUsedDefault = 1;

  auto result = WideCharToMultiByte(codePage, flags, wideStr, numChars, multiByteStr, multiByteSize, defaultChar, usedDefaultChar ? &localUsedDefault : nullptr);

  if (!result)
    throwLastWin32Error();

  if (usedDefaultChar)
    *usedDefaultChar = localUsedDefault != 0;

  return result;
}

} // end namespace curt
