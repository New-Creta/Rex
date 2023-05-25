

#pragma once

#include "rex_engine/diagnostics/logging/internal/common.h"

#include <ctime> // time_t

namespace rexlog
{
  namespace details
  {
    namespace os
    {

       rexlog::log_clock::time_point now() noexcept;

       tm localtime(const time_t& timeTt) noexcept;

       tm localtime() noexcept;

       tm gmtime(const time_t& timeTt) noexcept;

       tm gmtime() noexcept;

// eol definition
#if !defined(REXLOG_EOL)
  #ifdef _WIN32
    #define REXLOG_EOL "\r\n"
  #else
    #define REXLOG_EOL "\n"
  #endif
#endif

      constexpr static const char* default_eol = REXLOG_EOL;

// folder separator
#if !defined(REXLOG_FOLDER_SEPS)
  #ifdef _WIN32
    #define REXLOG_FOLDER_SEPS "\\/"
  #else
    #define REXLOG_FOLDER_SEPS "/"
  #endif
#endif

      constexpr static const rsl::string_view folder_seps          = REXLOG_FOLDER_SEPS;
      constexpr static const rsl::string_view folder_seps_filename = REXLOG_FILENAME_T(REXLOG_FOLDER_SEPS);

      // fopen_s on non windows for writing
       bool fopen_s(FILE** fp, const filename_t& filename, const filename_t& mode);

      // Remove filename. return 0 on success
       int remove(const filename_t& filename) noexcept;

      // Remove file if exists. return 0 on success
      // Note: Non atomic (might return failure to delete if concurrently deleted by other process/thread)
       int remove_if_exists(const filename_t& filename) noexcept;

       int rename(const filename_t& filename1, const filename_t& filename2) noexcept;

      // Return if file exists.
       bool path_exists(const filename_t& filename) noexcept;

      // Return file size according to open FILE* object
       size_t filesize(FILE* f);

      // Return utc offset in minutes or throw rexlog_ex on failure
       int utc_minutes_offset(const tm& tm = details::os::localtime());

      // Return current thread id as size_t
      // It exists because the rsl::this_thread::get_id() is much slower(especially
      // under VS 2013)
       size_t thread_id() noexcept;

      // Return current thread id as size_t (from thread local storage)
      

      // This is avoid msvc issue in sleep_for that happens if the clock changes.
      // See https://github.com/gabime/rexlog/issues/609
       void sleep_for_millis(unsigned int milliseconds) noexcept;

       rex::DebugString filename_to_str(const filename_t& filename);

       int pid() noexcept;

      // Determine if the terminal supports colors
      // Source: https://github.com/agauniyal/rang/
       bool is_color_terminal() noexcept;

      // Determine if the terminal attached
      // Source: https://github.com/agauniyal/rang/
       bool in_terminal(FILE* file) noexcept;

      // Return directory name from given path or empty string
      // "abc/file" => "abc"
      // "abc/" => "abc"
      // "abc" => ""
      // "abc///" => "abc//"
       filename_t dir_name(const filename_t& path);

      // Create a dir from the given path.
      // Return true if succeeded or if this dir already exists.
       bool create_dir(const filename_t& path);

      // non thread safe, cross platform getenv/getenv_s
      // return empty string if field not found
       rex::DebugString getenv(const char* field);

      // Do fsync by FILE objectpointer.
      // Return true on success.
       bool fsync(FILE* fp);

    } // namespace os
  }   // namespace details
} // namespace rexlog
