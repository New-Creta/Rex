

#pragma once

#include "rex_engine/debug_types.h"
#include "rex_engine/diagnostics/logging/internal/common.h"
#include "rex_std/bonus/string.h"

#include <corecrt_wtime.h>
#include <ctime> // time_t
#include <stdio.h>

namespace rex
{
  namespace log
  {
    namespace details
    {
      namespace os
      {
        rex::log::log_clock::time_point now() noexcept;

        tm localtime(const time_t& timeTt) noexcept;
        tm localtime() noexcept;

        tm gmtime(const time_t& timeTt) noexcept;
        tm gmtime() noexcept;

        // fopen_s on non windows for writing
        bool fopen_s(FILE** fp, rsl::string_view filename, const filename_t& mode);

        // Remove filename. return 0 on success
        int remove(const filename_t& filename) noexcept;

        // Return if file exists.
        bool path_exists(const filename_t& filename) noexcept;

        // Return file size according to open FILE* object
        size_t filesize(FILE* f);

        // Return utc offset in minutes or throw rexlog_ex on failure
        int utc_minutes_offset(const tm& tm = details::os::localtime());

        // Return current thread id as size_t
        // It exists because the rsl::this_thread::get_id() is much slower(especially under VS 2013)
        size_t thread_id() noexcept;

        // This is avoid msvc issue in sleep_for that happens if the clock changes.
        // See https://github.com/gabime/rexlog/issues/609
        void sleep_for_millis(unsigned int milliseconds) noexcept;

        rex::DebugString filename_to_str(const filename_t& filename);

        int pid() noexcept;

        // Return directory name from given path or empty string
        // "abc/file" => "abc"
        // "abc/" => "abc"
        // "abc" => ""
        // "abc///" => "abc//"
        filename_t dir_name(rsl::string_view path);

        // Create a dir from the given path.
        // Return true if succeeded or if this dir already exists.
        bool create_dir(const filename_t& path);

        // Do fsync by FILE objectpointer.
        // Return true on success.
        bool fsync(FILE* fp);

      } // namespace os
    }   // namespace details
  }     // namespace log
} // namespace rex
