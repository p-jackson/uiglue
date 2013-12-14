#include "error.h"

#include <exception>

static std::exception_ptr savedError;

namespace curt {

  void clearCurrentException() {
    savedError = {};
  }

  void saveCurrentException() {
    savedError = std::current_exception();
  }

  void throwSavedException() {
    if (savedError != std::exception_ptr())
      std::rethrow_exception(savedError);
  }

}
