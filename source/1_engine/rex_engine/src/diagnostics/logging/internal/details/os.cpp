#include "rex_engine/diagnostics/logging/internal/details/os.h"

#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/diagnostics/logging/internal/common.h"
#include "rex_engine/filesystem/filesystem_constants.h"

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

#else                 // unix

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

#endif                       // unix

#ifndef __has_feature        // Clang - feature checking macros.
  #define __has_feature(x) 0 // Compatibility with non-clang compilers.
#endif

// NOLINTBEGIN(misc-definitions-in-headers)

namespace rex
{
  namespace log
  {
    namespace details
    {
      namespace os
      {
        rex::log::log_clock::time_point now() noexcept
        {
          return log_clock::now();
        }
        tm localtime(const time_t& time) noexcept
        {
#ifdef _WIN32
          tm tm {};
          REX_MAYBE_UNUSED const errno_t res = ::localtime_s(&tm, &time);
          REX_ASSERT_X(res == 0, "failed to convert to local time");
#else
          tm tm;
          ::localtime_r(&time, &tm);
#endif
          return tm;
        }

        tm localtime() noexcept
        {
          const time_t now_t = ::time(nullptr);
          return localtime(now_t);
        }

        tm gmtime(const time_t& timeTt) noexcept
        {
#ifdef _WIN32
          tm tm {};
          REX_MAYBE_UNUSED const errno_t res = ::gmtime_s(&tm, &timeTt);
          REX_ASSERT_X(res == 0, "failed to convert to gm");
#else
          tm tm;
          ::gmtime_r(&time_tt, &tm);
#endif
          return tm;
        }

        tm gmtime() noexcept
        {
          const time_t now_t = ::time(nullptr);
          return gmtime(now_t);
        }

        // fopen_s on non windows for writing
        bool fopen_s(FILE** fp, rsl::string_view filename, const filename_t& mode)
        {
#ifdef _WIN32
          *fp = ::_fsopen((filename.data()), mode.c_str(), _SH_DENYNO);
#else
          *fp = ::fopen((filename.c_str()), mode.c_str());
#endif
          return *fp == nullptr;
        }

        int remove(const filename_t& filename) noexcept
        {
          return ::remove(filename.c_str());
        }

        // Return true if path exists (file or directory)
        bool path_exists(const filename_t& filename) noexcept
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
        size_t filesize(FILE* f)
        {
          if(f == nullptr)
          {
            printf("Failed getting file size. fd is null");
          }
#if defined(_WIN32) && !defined(__CYGWIN__)
          const int fd = ::_fileno(f);
  #if defined(_WIN64) // 64 bits
          const __int64 ret = ::_filelengthi64(fd);
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
          printf("Failed getting file size from fd: %d", errno);
          return 0; // will not be reached.
        }

#ifdef _MSC_VER
  #pragma warning(pop)
#endif

        // Return utc offset in minutes or throw rexlog_ex on failure
        int utc_minutes_offset(const tm& tm)
        {
#ifdef _WIN32
  #if _WIN32_WINNT < _WIN32_WINNT_WS08
          TIME_ZONE_INFORMATION tzinfo;
          auto rv = ::GetTimeZoneInformation(&tzinfo);
  #else
          DYNAMIC_TIME_ZONE_INFORMATION tzinfo;
          auto rv = ::GetDynamicTimeZoneInformation(&tzinfo);
  #endif
          if(rv == TIME_ZONE_ID_INVALID)
            printf("Failed getting timezone info: %d", errno);

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

  #if defined(sun) || defined(__sun) || defined(_AIX) || (defined(__NEWLIB__) && !defined(__TM_GMtOFF)) || (!defined(_BSD_SOURCE) && !defined(_GNU_SOURCE))
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
        size_t thread_id_impl() noexcept
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
        size_t thread_id() noexcept
        {
          static thread_local const size_t s_tid = thread_id_impl();
          return s_tid;
        }

        // This is avoid msvc issue in sleep_for that happens if the clock changes.
        // See https://github.com/gabime/spdlog/issues/609
        void sleep_for_millis(unsigned int milliseconds) noexcept
        {
#if defined(_WIN32)
          ::Sleep(milliseconds);
#else
          rsl::this_thread::sleep_for(rsl::chrono::milliseconds(milliseconds));
#endif
        }

        rex::debug_string filename_to_str(const filename_t& filename)
        {
          return rex::debug_string(filename);
        }

        int pid() noexcept
        {
#ifdef _WIN32
          return conditional_static_cast<int>(::GetCurrentProcessId());
#else
          return conditional_static_cast<int>(::getpid());
#endif
        }

        // return true on success
        static bool mkdir_impl(const filename_t& path) // NOLINT(misc-use-anonymous-namespace)
        {
#ifdef _WIN32
          return ::_mkdir(path.c_str()) == 0;
#else
          return ::mkdir(path.c_str(), mode_t(0755)) == 0;
#endif
        }

        // create the given directory - and all directories leading to it
        // return true on success or if the directory already exists
        bool create_dir(const filename_t& path)
        {
          if(path_exists(path))
          {
            return true;
          }

          if(path.empty())
          {
            return false;
          }

          s32 search_offset = 0;
          do // NOLINT(cppcoreguidelines-avoid-do-while)
          {
            auto token_pos = path.find_first_of(rex::g_folder_seps_filename, static_cast<count_t>(search_offset));
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
          } while(static_cast<count_t>(search_offset) < path.size());

          return true;
        }

        // Return directory name from given path or empty string
        // "abc/file" => "abc"
        // "abc/" => "abc"
        // "abc" => ""
        // "abc///" => "abc//"
        filename_t dir_name(rsl::string_view path)
        {
          auto pos = path.find_last_of(rex::g_folder_seps_filename);
          return pos != filename_t::npos() ? filename_t(path.substr(0, pos)) : filename_t {};
        }

        // Do fsync by FILE handlerpointer
        // Return true on success
        bool fsync(FILE* fp)
        {
#ifdef _WIN32
          return FlushFileBuffers(reinterpret_cast<HANDLE>(_get_osfhandle(_fileno(fp)))) != 0; // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast, performance-no-int-to-ptr)
#else
          return ::fsync(fileno(fp)) == 0;
#endif
        }

      } // namespace os
    }   // namespace details
  }     // namespace log
} // namespace rex

  // NOLINTEND(misc-definitions-in-headers)