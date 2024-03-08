#pragma once

#include "rex_std/string_view.h"
#include "rex_std/string.h"
#include "rex_std/vector.h"
#include "rex_std/bonus/time/timepoint.h"
#include "rex_std/bonus/utility/yes_no.h"

#include "rex_engine/diagnostics/logging/log_macros.h"

namespace rex
{
  namespace directory
  {
    DEFINE_LOG_CATEGORY(DirectoryLog, rex::LogVerbosity::Log);

    // Create a new directory
    void create(rsl::string_view path);
    // Delete a directory
    void del(rsl::string_view path);
    // Return if a directory exists
    bool exists(rsl::string_view path);
    // Copy a directory and its content
    void copy(rsl::string_view src, rsl::string_view dst);
    // Move/Rename a directory
    void move(rsl::string_view src, rsl::string_view dst);

    DEFINE_YES_NO_ENUM(ListRecusrive);
    // List all entries under a directory
    rsl::vector<rsl::string> list_entries(rsl::string_view path, ListRecusrive listRecursive = ListRecusrive::no);
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
  }
}