#ifndef CURT_ERROR_H
#define CURT_ERROR_H

namespace curt {

  void clearCurrentException();
  void saveCurrentException();
  void throwSavedException();

}

#endif
