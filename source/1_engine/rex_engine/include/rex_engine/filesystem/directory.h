#pragma once

#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/diagnostics/error.h"
#include "rex_std/bonus/time/timepoint.h"
#include "rex_std/bonus/utility/yes_no.h"
#include "rex_std/string.h"
#include "rex_std/string_view.h"
#include "rex_std/vector.h"

namespace rex
{
  namespace directory
  {
    // Create a new directory
    Error create(rsl::string_view path);
    // Create a directory recursively, creating all sub directories until the leaf dir
    Error create_recursive(rsl::string_view path);
    // Delete a directory
    Error del(rsl::string_view path);
    // Delete a directory recursively, including all files and sub folders
    Error del_recusrive(rsl::string_view path);
    // Return if a directory exists
    bool exists(rsl::string_view path);
    // Copy a directory and its content
    Error copy(rsl::string_view src, rsl::string_view dst);
    // Move/Rename a directory
    Error move(rsl::string_view src, rsl::string_view dst);

    DEFINE_YES_NO_ENUM(Recursive);
    // Get the size of all the files in the directory
    // possible going recursive over all the sub folders as well
    rsl::memory_size size(rsl::string_view path, Recursive goRecursive = Recursive::no);
    // List all entries under a directory
    rsl::vector<rsl::string> list_entries(rsl::string_view path, Recursive listRecursive = Recursive::no);
    // List all directories under a directory
    rsl::vector<rsl::string> list_dirs(rsl::string_view path);
    // List all files under a directory
    rsl::vector<rsl::string> list_files(rsl::string_view path);

    // Return the creation time of a directory
    rsl::time_point creation_time(rsl::string_view path);
    // Return the access time of a directory
    rsl::time_point access_time(rsl::string_view path);
    // Return the modification time of a directory
    rsl::time_point modification_time(rsl::string_view path);
  } // namespace directory
} // namespace rex