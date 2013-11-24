//===-- Boost.Filesystem wrapper ------------------------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <boost/filesystem.hpp>

namespace filesystem {
  using boost::filesystem::path;
  using boost::filesystem::is_directory;
  using boost::filesystem::is_regular_file;
  using boost::filesystem::absolute;
}

#endif
