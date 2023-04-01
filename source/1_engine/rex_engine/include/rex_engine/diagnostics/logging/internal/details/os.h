

#pragma once

#include <ctime> // time_t
#include <rex_engine/diagnostics/logging/internal/common.h>

namespace rexlog
{
  namespace details
  {
    namespace os
    {

      REXLOG_API rexlog::log_clock::time_point now() REXLOG_NOEXCEPT;

      REXLOG_API tm localtime(const time_t& time_tt) REXLOG_NOEXCEPT;

      REXLOG_API tm localtime() REXLOG_NOEXCEPT;

      REXLOG_API tm gmtime(const time_t& time_tt) REXLOG_NOEXCEPT;

      REXLOG_API tm gmtime() REXLOG_NOEXCEPT;

// eol definition
#if !defined(REXLOG_EOL)
  #ifdef _WIN32
    #define REXLOG_EOL "\r\n"
  #else
    #define REXLOG_EOL "\n"
  #endif
#endif

      REXLOG_CONSTEXPR static const char* default_eol = REXLOG_EOL;

// folder separator
#if !defined(REXLOG_FOLDER_SEPS)
  #ifdef _WIN32
    #define REXLOG_FOLDER_SEPS "\\/"
  #else
    #define REXLOG_FOLDER_SEPS "/"
  #endif
#endif

      REXLOG_CONSTEXPR static const char folder_seps[]                            = REXLOG_FOLDER_SEPS;
      REXLOG_CONSTEXPR static const filename_t::value_type folder_seps_filename[] = REXLOG_FILENAME_T(REXLOG_FOLDER_SEPS);

      // fopen_s on non windows for writing
      REXLOG_API bool fopen_s(FILE** fp, const filename_t& filename, const filename_t& mode);

      // Remove filename. return 0 on success
      REXLOG_API int remove(const filename_t& filename) REXLOG_NOEXCEPT;

      // Remove file if exists. return 0 on success
      // Note: Non atomic (might return failure to delete if concurrently deleted by other process/thread)
      REXLOG_API int remove_if_exists(const filename_t& filename) REXLOG_NOEXCEPT;

      REXLOG_API int rename(const filename_t& filename1, const filename_t& filename2) REXLOG_NOEXCEPT;

      // Return if file exists.
      REXLOG_API bool path_exists(const filename_t& filename) REXLOG_NOEXCEPT;

      // Return file size according to open FILE* object
      REXLOG_API size_t filesize(FILE* f);

      // Return utc offset in minutes or throw rexlog_ex on failure
      REXLOG_API int utc_minutes_offset(const tm& tm = details::os::localtime());

      // Return current thread id as size_t
      // It exists because the rsl::this_thread::get_id() is much slower(especially
      // under VS 2013)
      REXLOG_API size_t _thread_id() REXLOG_NOEXCEPT;

      // Return current thread id as size_t (from thread local storage)
      REXLOG_API size_t thread_id() REXLOG_NOEXCEPT;

      // This is avoid msvc issue in sleep_for that happens if the clock changes.
      // See https://github.com/gabime/rexlog/issues/609
      REXLOG_API void sleep_for_millis(unsigned int milliseconds) REXLOG_NOEXCEPT;

      REXLOG_API rex::DebugString filename_to_str(const filename_t& filename);

      REXLOG_API int pid() REXLOG_NOEXCEPT;

      // Determine if the terminal supports colors
      // Source: https://github.com/agauniyal/rang/
      REXLOG_API bool is_color_terminal() REXLOG_NOEXCEPT;

      // Determine if the terminal attached
      // Source: https://github.com/agauniyal/rang/
      REXLOG_API bool in_terminal(FILE* file) REXLOG_NOEXCEPT;

      // Return directory name from given path or empty string
      // "abc/file" => "abc"
      // "abc/" => "abc"
      // "abc" => ""
      // "abc///" => "abc//"
      REXLOG_API filename_t dir_name(const filename_t& path);

      // Create a dir from the given path.
      // Return true if succeeded or if this dir already exists.
      REXLOG_API bool create_dir(const filename_t& path);

      // non thread safe, cross platform getenv/getenv_s
      // return empty string if field not found
      REXLOG_API rex::DebugString getenv(const char* field);

      // Do fsync by FILE objectpointer.
      // Return true on success.
      REXLOG_API bool fsync(FILE* fp);

    } // namespace os
  }   // namespace details
} // namespace rexlog
