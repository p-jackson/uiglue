//===-- Error handling helpers --------------------------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//

#ifndef CURT_ERROR_H
#define CURT_ERROR_H

namespace curt {

  void clearCurrentException();
  void saveCurrentException();
  void throwSavedException();

  void throwWin32Error(unsigned long error);
  void throwIfWin32Error();
  void throwLastWin32Error();

}

#endif
