#pragma once

#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/diagnostics/error.h"
#include "rex_engine/memory/blob.h"
#include "rex_std/bonus/time/timepoint.h"
#include "rex_std/bonus/utility/yes_no.h"
#include "rex_std/string.h"
#include "rex_std/string_view.h"
#include "rex_std/vector.h"

namespace rex
{
  namespace file
  {
    DEFINE_YES_NO_ENUM(OverwriteIfExist);

    // Read from a file
    memory::Blob read_file(rsl::string_view path);
    // Save content to a file
    Error save_to_file(rsl::string_view filepath, const void* data, card64 size);
    // Append lines to a file
    Error append_lines(rsl::string_view path, const rsl::vector<rsl::string>& lines);
    // Append text to a file
    Error append_text(rsl::string_view path, rsl::string_view txt);
    // Trunc a file, removing all content
    Error trunc(rsl::string_view path);
    // Copy a file, overwiting an existing one is possible
    Error copy(rsl::string_view src, rsl::string_view dst, OverwriteIfExist overwriteIfExist = OverwriteIfExist::no);
    // Move/Rename a file, overwriting an existing one is possible
    Error move(rsl::string_view src, rsl::string_view dst);
    // Create a new empty file
    Error create(rsl::string_view path);
    // Delete a file
    Error del(rsl::string_view path);
    // return the file size
    card64 size(rsl::string_view path);
    // Check if a file exists
    bool exists(rsl::string_view path);
    // Check if a file is marked read only
    bool is_readonly(rsl::string_view path);
    // Set a file to be readonly
    Error set_readonly(rsl::string_view path);
    // Remove the readonly flag of a file
    Error remove_readonly(rsl::string_view path);
    // Return the creation time of a file
    rsl::time_point creation_time(rsl::string_view path);
    // Return the access time of a file
    rsl::time_point access_time(rsl::string_view path);
    // Return the modification time of a file
    rsl::time_point modification_time(rsl::string_view path);
  } // namespace file

} // namespace rex