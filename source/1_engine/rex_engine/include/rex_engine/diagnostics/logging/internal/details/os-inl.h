

#pragma once

#include "rex_engine/diagnostics/logging/internal/common.h"
#include "rex_engine/diagnostics/logging/internal/details/os.h"
#include <algorithm>
#include <array>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <thread>

#ifdef _WIN32

  #include "rex_engine/diagnostics/logging/internal/details/windows_include.h"
  #include <Windows.h>
  #include <fileapi.h> // for FlushFileBuffers
  #include <io.h>      // for _get_osfhandle, _isatty, _fileno
  #include <process.h> // for _get_pid

  #ifdef __MINGW32__
    #include <share.h>
  #endif

  #include <direct.h> // for _mkdir/_wmkdir

#else // unix

  #include <fcntl.h>
  #include <unistd.h>

  #ifdef __linux__
    #include <sys/syscall.h> //Use gettid() syscall under linux to get thread id

  #elif defined(_AIX)
    #include <pthread.h> // for pthread_getthrds_np

  #elif defined(__DragonFly__) || defined(__FreeBSD__)
    #include <pthread_np.h> // for pthread_getthreadid_np

  #elif defined(__NetBSD__)
    #include <lwp.h> // for _lwp_self

  #elif defined(__sun)
    #include <thread.h> // for thr_self
  #endif

#endif // unix

#ifndef __has_feature        // Clang - feature checking macros.
  #define __has_feature(x) 0 // Compatibility with non-clang compilers.
#endif

namespace rexlog
{
  namespace details
  {
    namespace os
    {

      REXLOG_INLINE rexlog::log_clock::time_point now() REXLOG_NOEXCEPT
      {
#if defined __linux__ && defined REXLOG_CLOCK_COARSE
        timespec ts;
        ::clock_gettime(CLOCK_REALTIME_COARSE, &ts);
        return rsl::chrono::time_point<log_clock, typename log_clock::duration>(rsl::chrono::duration_cast<typename log_clock::duration>(rsl::chrono::seconds(ts.tv_sec) + rsl::chrono::nanoseconds(ts.tv_nsec)));

#else
        return log_clock::now();
#endif
      }
      REXLOG_INLINE tm localtime(const time_t& time) REXLOG_NOEXCEPT
      {
#ifdef _WIN32
        tm tm{};
        REX_ASSERT_X(::localtime_s(&tm, &time) == 0, "failed to convert to local time");
#else
        tm tm;
        ::localtime_r(&time, &tm);
#endif
        return tm;
      }

      REXLOG_INLINE tm localtime() REXLOG_NOEXCEPT
      {
        time_t const now_t = ::time(nullptr);
        return localtime(now_t);
      }

      REXLOG_INLINE tm gmtime(const time_t& timeTt) REXLOG_NOEXCEPT
      {
#ifdef _WIN32
        tm tm{};
        REX_ASSERT_X(::gmtime_s(&tm, &timeTt), "failed to convert to gm");
#else
        tm tm;
        ::gmtime_r(&time_tt, &tm);
#endif
        return tm;
      }

      REXLOG_INLINE tm gmtime() REXLOG_NOEXCEPT
      {
        time_t const now_t = ::time(nullptr);
        return gmtime(now_t);
      }

      // fopen_s on non windows for writing
      REXLOG_INLINE bool fopen_s(FILE** fp, const filename_t& filename, const filename_t& mode)
      {
#ifdef _WIN32
        *fp = ::_fsopen((filename.c_str()), mode.c_str(), _SH_DENYNO);
  #if defined(REXLOG_PREVENT_CHILD_FD)
        if(*fp != nullptr)
        {
          auto file_handle = reinterpret_cast<HANDLE>(_get_osfhandle(::_fileno(*fp)));
          if(!::SetHandleInformation(file_handle, HANDLE_FLAG_INHERIT, 0))
          {
            ::fclose(*fp);
            *fp = nullptr;
          }
        }
  #endif
#else // unix
  #if defined(REXLOG_PREVENT_CHILD_FD)
        const int mode_flag = mode == REXLOG_FILENAME_T("ab") ? O_APPEND : O_TRUNC;
        const int fd        = ::open((filename.c_str()), O_CREAT | O_WRONLY | O_CLOEXEC | mode_flag, mode_t(0644));
        if(fd == -1)
        {
          return true;
        }
        *fp = ::fdopen(fd, mode.c_str());
        if(*fp == nullptr)
        {
          ::close(fd);
        }
  #else
        *fp    = ::fopen((filename.c_str()), mode.c_str());
  #endif
#endif

        return *fp == nullptr;
      }

      REXLOG_INLINE int remove(const filename_t& filename) REXLOG_NOEXCEPT
      {
        return ::remove(filename.c_str());
      }

      REXLOG_INLINE int remove_if_exists(const filename_t& filename) REXLOG_NOEXCEPT
      {
        return path_exists(filename) ? remove(filename) : 0;
      }

      REXLOG_INLINE int rename(const filename_t& filename1, const filename_t& filename2) REXLOG_NOEXCEPT
      {
        return ::rename(filename1.c_str(), filename2.c_str());
      }

      // Return true if path exists (file or directory)
      REXLOG_INLINE bool path_exists(const filename_t& filename) REXLOG_NOEXCEPT
      {
#ifdef _WIN32
        auto attribs = ::GetFileAttributesA(filename.c_str());
        return attribs != INVALID_FILE_ATTRIBUTES;
#else // common linux/unix all have the stat system call
        struct stat buffer;
        return (::stat(filename.c_str(), &buffer) == 0);
#endif
      }

#ifdef _MSC_VER
  // avoid warning about unreachable statement at the end of filesize()
  #pragma warning(push)
  #pragma warning(disable : 4702)
#endif

      // Return file size according to open FILE* object
      REXLOG_INLINE size_t filesize(FILE* f)
      {
        if(f == nullptr)
        {
          throw_rexlog_ex("Failed getting file size. fd is null");
        }
#if defined(_WIN32) && !defined(__CYGWIN__)
        int const fd = ::_fileno(f);
  #if defined(_WIN64) // 64 bits
        __int64 const ret = ::_filelengthi64(fd);
        if(ret >= 0)
        {
          return static_cast<size_t>(ret);
        }

  #else // windows 32 bits
        long ret = ::_filelength(fd);
        if(ret >= 0)
        {
          return static_cast<size_t>(ret);
        }
  #endif

#else // unix
  // OpenBSD and AIX doesn't compile with :: before the fileno(..)
  #if defined(__OpenBSD__) || defined(_AIX)
        int fd = fileno(f);
  #else
        int fd = ::fileno(f);
  #endif
  // 64 bits(but not in osx, linux/musl or cygwin, where fstat64 is deprecated)
  #if((defined(__linux__) && defined(__GLIBC__)) || defined(__sun) || defined(_AIX)) && (defined(__LP64__) || defined(_LP64))
        struct stat64 st;
        if(::fstat64(fd, &st) == 0)
        {
          return static_cast<size_t>(st.st_size);
        }
  #else // other unix or linux 32 bits or cygwin
        struct stat st;
        if(::fstat(fd, &st) == 0)
        {
          return static_cast<size_t>(st.st_size);
        }
  #endif
#endif
        throw_rexlog_ex("Failed getting file size from fd", errno);
        return 0; // will not be reached.
      }

#ifdef _MSC_VER
  #pragma warning(pop)
#endif

      // Return utc offset in minutes or throw rexlog_ex on failure
      REXLOG_INLINE int utc_minutes_offset(const tm& tm)
      {
#ifdef _WIN32
  #if _WIN32_WINNT < _WIN32_WINNT_WS08
        TIME_ZONE_INFORMATION tzinfo;
        auto rv = ::GetTimeZoneInformation(&tzinfo);
  #else
        DYNAMIC_TIME_ZONE_INFORMATION tzinfo;
        auto rv    = ::GetDynamicTimeZoneInformation(&tzinfo);
  #endif
        if(rv == TIME_ZONE_ID_INVALID)
          throw_rexlog_ex("Failed getting timezone info. ", errno);

        int offset = -tzinfo.Bias;
        if(tm.tm_isdst != 0)
        {
          offset -= tzinfo.DaylightBias;
        }
        else
        {
          offset -= tzinfo.StandardBias;
        }
        return offset;
#else

  #if defined(sun) || defined(__sun) || defined(_AIX) || (defined(__NEWLIB__) && !defined(__TM_GMTOFF)) || (!defined(_BSD_SOURCE) && !defined(_GNU_SOURCE))
        // 'tm_gmtoff' field is BSD extension and it's missing on SunOS/Solaris
        struct helper
        {
          static long int calculate_gmt_offset(const tm& localtm = details::os::localtime(), const tm& gmtm = details::os::gmtime())
          {
            int local_year = localtm.tm_year + (1900 - 1);
            int gmt_year   = gmtm.tm_year + (1900 - 1);

            long int days = (
                // difference in day of year
                localtm.tm_yday -
                gmtm.tm_yday

                // + intervening leap days
                + ((local_year >> 2) - (gmt_year >> 2)) - (local_year / 100 - gmt_year / 100) +
                ((local_year / 100 >> 2) - (gmt_year / 100 >> 2))

                // + difference in years * 365 */
                + static_cast<long int>(local_year - gmt_year) * 365);

            long int hours = (24 * days) + (localtm.tm_hour - gmtm.tm_hour);
            long int mins  = (60 * hours) + (localtm.tm_min - gmtm.tm_min);
            long int secs  = (60 * mins) + (localtm.tm_sec - gmtm.tm_sec);

            return secs;
          }
        };

        auto offset_seconds = helper::calculate_gmt_offset(tm);
  #else
        auto offset_seconds = tm.tm_gmtoff;
  #endif

        return static_cast<int>(offset_seconds / 60);
#endif
      }

      // Return current thread id as size_t
      // It exists because the rsl::this_thread::get_id() is much slower(especially
      // under VS 2013)
      REXLOG_INLINE size_t thread_id_impl() REXLOG_NOEXCEPT
      {
#ifdef _WIN32
        return static_cast<size_t>(::GetCurrentThreadId());
#elif defined(__linux__)
  #if defined(__ANDROID__) && defined(__ANDROID_API__) && (__ANDROID_API__ < 21)
    #define SYS_gettid __NR_gettid
  #endif
        return static_cast<size_t>(::syscall(SYS_gettid));
#elif defined(_AIX)
        struct __pthrdsinfo buf;
        int reg_size = 0;
        pthread_t pt = pthread_self();
        int retval   = pthread_getthrds_np(&pt, PTHRDSINFO_QUERY_TID, &buf, sizeof(buf), NULL, &reg_size);
        int tid      = (!retval) ? buf.__pi_tid : 0;
        return static_cast<size_t>(tid);
#elif defined(__DragonFly__) || defined(__FreeBSD__)
        return static_cast<size_t>(::pthread_getthreadid_np());
#elif defined(__NetBSD__)
        return static_cast<size_t>(::_lwp_self());
#elif defined(__OpenBSD__)
        return static_cast<size_t>(::getthrid());
#elif defined(__sun)
        return static_cast<size_t>(::thr_self());
#elif __APPLE__
        uint64_t tid;
        pthread_threadid_np(nullptr, &tid);
        return static_cast<size_t>(tid);
#else // Default to standard C++11 (other Unix)
        return static_cast<size_t>(rsl::hash<rsl::thread::id>()(rsl::this_thread::get_id()));
#endif
      }

      // Return current thread id as size_t (from thread local storage)
      REXLOG_INLINE size_t thread_id() REXLOG_NOEXCEPT
      {
#if defined(REXLOG_NO_TLS)
        return _thread_id();
#else // cache thread id in tls
        static thread_local const size_t s_tid = thread_id_impl();
        return s_tid;
#endif
      }

      // This is avoid msvc issue in sleep_for that happens if the clock changes.
      // See https://github.com/gabime/spdlog/issues/609
      REXLOG_INLINE void sleep_for_millis(unsigned int milliseconds) REXLOG_NOEXCEPT
      {
#if defined(_WIN32)
        ::Sleep(milliseconds);
#else
        rsl::this_thread::sleep_for(rsl::chrono::milliseconds(milliseconds));
#endif
      }

      REXLOG_INLINE rex::DebugString filename_to_str(const filename_t& filename)
      {
        return rex::DebugString(filename);
      }

      REXLOG_INLINE int pid() REXLOG_NOEXCEPT
      {
#ifdef _WIN32
        return conditional_static_cast<int>(::GetCurrentProcessId());
#else
        return conditional_static_cast<int>(::getpid());
#endif
      }

      // Determine if the terminal supports colors
      // Based on: https://github.com/agauniyal/rang/
      REXLOG_INLINE bool is_color_terminal() REXLOG_NOEXCEPT
      {
#ifdef _WIN32
        return true;
#else

        static const bool result = []()
        {
          const char* env_colorterm_p = rsl::getenv("COLORTERM");
          if(env_colorterm_p != nullptr)
          {
            return true;
          }

          static constexpr rsl::array<const char*, 16> terms = {{"ansi", "color", "console", "cygwin", "gnome", "konsole", "kterm", "linux", "msys", "putty", "rxvt", "screen", "vt100", "xterm", "alacritty", "vt102"}};

          const char* env_term_p = rsl::getenv("TERM");
          if(env_term_p == nullptr)
          {
            return false;
          }

          return rsl::any_of(terms.begin(), terms.end(), [&](const char* term) { return rsl::strstr(env_term_p, term) != nullptr; });
        }();

        return result;
#endif
      }

      // Determine if the terminal attached
      // Source: https://github.com/agauniyal/rang/
      REXLOG_INLINE bool in_terminal(FILE* file) REXLOG_NOEXCEPT
      {
#ifdef _WIN32
        return ::_isatty(_fileno(file)) != 0;
#else
        return ::isatty(fileno(file)) != 0;
#endif
      }

      // return true on success
      static REXLOG_INLINE bool mkdir_impl(const filename_t& path)
      {
#ifdef _WIN32
        return ::_mkdir(path.c_str()) == 0;
#else
        return ::mkdir(path.c_str(), mode_t(0755)) == 0;
#endif
      }

      // create the given directory - and all directories leading to it
      // return true on success or if the directory already exists
      REXLOG_INLINE bool create_dir(const filename_t& path)
      {
        if(path_exists(path))
        {
          return true;
        }

        if(path.empty())
        {
          return false;
        }

        size_t search_offset = 0;
        do
        {
          auto token_pos = path.find_first_of(rsl::string_view(folder_seps_filename), static_cast<count_t>(search_offset));
          // treat the entire path as a folder if no folder separator not found
          if(token_pos == filename_t::npos())
          {
            token_pos = path.size();
          }

          auto subdir = path.substr(0, token_pos);

          if(!subdir.empty() && !path_exists(filename_t(subdir)) && !mkdir_impl(filename_t(subdir)))
          {
            return false; // return error if failed creating dir
          }
          search_offset = token_pos + 1;
        } while(search_offset < path.size());

        return true;
      }

      // Return directory name from given path or empty string
      // "abc/file" => "abc"
      // "abc/" => "abc"
      // "abc" => ""
      // "abc///" => "abc//"
      REXLOG_INLINE filename_t dir_name(const filename_t& path)
      {
        auto pos = path.find_last_of(rsl::string_view(folder_seps_filename));
        return pos != filename_t::npos() ? filename_t(path.substr(0, pos)) : filename_t {};
      }

      inline rex::DebugString REXLOG_INLINE getenv(const char* field)
      {
#if defined(_MSC_VER)
  #if defined(__cplusplus_winrt)
        return rex::DebugString {}; // not supported under uwp
  #else
        size_t len = 0;
        rsl::array<char, 128> buf;
        bool const ok = ::getenv_s(&len, buf.data(), buf.size(), field) == 0;
        return ok ? rex::DebugString(buf.data()) : rex::DebugString {};
  #endif
#else // revert to getenv
        char* buf = ::getenv(field);
        return buf ? buf : rex::DebugString {};
#endif
      }

      // Do fsync by FILE handlerpointer
      // Return true on success
      REXLOG_INLINE bool fsync(FILE* fp)
      {
#ifdef _WIN32
        return FlushFileBuffers(reinterpret_cast<HANDLE>(_get_osfhandle(_fileno(fp)))) != 0; // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast, performance-no-int-to-ptr)
#else
        return ::fsync(fileno(fp)) == 0;
#endif
      }

    } // namespace os
  }   // namespace details
} // namespace rexlog
