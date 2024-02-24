#pragma once

#include "rex_std/string.h"
#include "rex_std/string_view.h"
#include "rex_std/vector.h"
#include "rex_std/bonus/utility/yes_no.h"
#include "rex_std/bonus/time/timepoint.h"

namespace rex
{
  DEFINE_YES_NO_ENUM(OverwriteIfExist);

  // Append lines to a file
  void append_lines(rsl::string_view path, const rsl::vector<rsl::string>& lines);
  // Append text to a file
  void append_text(rsl::string_view path, rsl::string_view txt);
  // Trunc a file, removing all content
  void trunc(rsl::string_view path);
  // Copy a file, overwiting an existing one is possible
  void copy(rsl::string_view src, rsl::string_view dst, OverwriteIfExist overwriteIfExist = OverwriteIfExist::no);
  // Move/Rename a file, overwriting an existing one is possible
  void move(rsl::string_view src, rsl::string_view dst, OverwriteIfExist overwriteIfExist = OverwriteIfExist::no);
  // Create a new empty file
  void create(rsl::string_view path);
  // Delete a file
  void del(rsl::string_view path);
  // Check if a file exists
  bool exists(rsl::string_view path);
  // Check if a file is marked read only
  bool is_readonly(rsl::string_view path);
  // Set a file to be readonly
  void set_readonly(rsl::string_view path);
  // Remove the readonly flag of a file
  void remove_readonly(rsl::string_view path);
  // Return the creation time of a file
  rsl::time_point creation_time(rsl::string_view path);
  // Return the access time of a file
  rsl::time_point access_time(rsl::string_view path);
  // Return the modification time of a file
  rsl::time_point modification_time(rsl::string_view path);

}