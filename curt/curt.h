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

void destroyWindow(HandleOr<HWND> wnd);
std::intptr_t dialogBoxParam(HINSTANCE hInst, StrOrId templateName, HandleOr<HWND> parent, DLGPROC proc, std::intptr_t param);
LRESULT dispatchMessage(const MSG* msg);
void endDialog(HandleOr<HWND> dlg, std::intptr_t result);
bool getMessage(MSG* msg, HandleOr<HWND> wnd, unsigned int msgFilterMin, unsigned int msgFilterMax);
HACCEL loadAccelerators(HINSTANCE hInst, StrOrId tableName);
int loadString(HINSTANCE hInst, unsigned int id, wchar_t* buffer, int bufferSize);
int messageBox(HandleOr<HWND> parent, String text, String caption, unsigned int type);
int multiByteToWideChar(unsigned int codePage, unsigned long flags, const char* multiByteStr, int multiByteSize, wchar_t* wideStr, int numChars);
void setWindowText(HandleOr<HWND> wnd, String str);
bool showWindow(HandleOr<HWND> wnd, int showCmd);
int translateAccelerator(HandleOr<HWND> hWnd, HACCEL hAccTable, MSG* msg);
bool translateMessage(const MSG* msg);
void updateWindow(HandleOr<HWND> wnd);
int wideCharToMultiByte(unsigned int codePage, unsigned long flags, const wchar_t* wideStr, int numChars, char* multiByteStr, int multiByteSize, const char* defaultChar, bool* usedDefaultChar);

}

#endif
