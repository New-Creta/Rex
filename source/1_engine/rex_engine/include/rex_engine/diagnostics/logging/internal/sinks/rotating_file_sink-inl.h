

#pragma once

#include "rex_engine/diagnostics/logging/internal/common.h"
#include "rex_engine/diagnostics/logging/internal/details/file_helper.h"
#include "rex_engine/diagnostics/logging/internal/details/null_mutex.h"
#include "rex_engine/diagnostics/logging/internal/sinks/rotating_file_sink.h"

namespace rexlog
{
  namespace sinks
  {

    template <typename Mutex>
    REXLOG_INLINE RotatingFileSink<Mutex>::RotatingFileSink(filename_t baseFileName, rsl::size_t maxSize, rsl::size_t maxFiles, bool rotateOnOpen, const FileEventHandlers& eventHandlers)
        : m_base_filename(rsl::move(baseFileName))
        , m_max_size(maxSize)
        , m_max_files(maxFiles)
        , m_file_helper {eventHandlers}
    {
      if(maxSize == 0)
      {
        throw_rexlog_ex(rex::DebugString("rotating sink constructor: max_size arg cannot be zero"));
      }

      if(maxFiles > 200000)
      {
        throw_rexlog_ex(rex::DebugString("rotating sink constructor: max_files arg cannot exceed 200000"));
      }
      m_file_helper.open(calc_filename(m_base_filename, 0));
      m_current_size = m_file_helper.size(); // expensive. called only once
      if(rotateOnOpen && m_current_size > 0)
      {
        rotate_impl();
        m_current_size = 0;
      }
    }

    // calc filename according to index and file extension if exists.
    // e.g. calc_filename("logs/mylog.txt, 3) => "logs/mylog.3.txt".
    template <typename Mutex>
    REXLOG_INLINE filename_t RotatingFileSink<Mutex>::calc_filename(const filename_t& filename, rsl::size_t index)
    {
      if(index == 0u)
      {
        return filename_t(filename);
      }

      details::FilenameWithExtension basename_ext_tuple = details::FileHelper::split_by_extension(filename);
      return filename_t(fmt_lib::format(REXLOG_FILENAME_T("{}.{}{}"), basename_ext_tuple.filename, index, basename_ext_tuple.ext));
    }

    template <typename Mutex>
    REXLOG_INLINE filename_t RotatingFileSink<Mutex>::filename()
    {
      rsl::unique_lock<Mutex> const lock(BaseSink<Mutex>::mutex());
      return m_file_helper.filename();
    }

    template <typename Mutex>
    REXLOG_INLINE void RotatingFileSink<Mutex>::sink_it_impl(const details::LogMsg& msg)
    {
      memory_buf_t formatted;
      BaseSink<Mutex>::formatter()->format(msg, formatted);
      auto new_size = m_current_size + formatted.size();

      // rotate if the new estimated file size exceeds max size.
      // rotate only if the real size > 0 to better deal with full disk (see issue #2261).
      // we only check the real size when new_size > max_size_ because it is relatively expensive.
      if(new_size > m_max_size)
      {
        m_file_helper.flush();
        if(m_file_helper.size() > 0)
        {
          rotate_impl();
          new_size = formatted.size();
        }
      }
      m_file_helper.write(formatted);
      m_current_size = new_size;
    }

    template <typename Mutex>
    REXLOG_INLINE void RotatingFileSink<Mutex>::flush_impl()
    {
      m_file_helper.flush();
    }

    // Rotate files:
    // log.txt -> log.1.txt
    // log.1.txt -> log.2.txt
    // log.2.txt -> log.3.txt
    // log.3.txt -> delete
    template <typename Mutex>
    REXLOG_INLINE void RotatingFileSink<Mutex>::rotate_impl()
    {
      using details::os::filename_to_str;
      using details::os::path_exists;

      m_file_helper.close();
      for(auto i = m_max_files; i > 0; --i)
      {
        filename_t const src = calc_filename(m_base_filename, i - 1);
        if(!path_exists(src))
        {
          continue;
        }
        filename_t const target = calc_filename(m_base_filename, i);

        if(!rename_file_impl(src, target))
        {
          // if failed try again after a small delay.
          // this is a workaround to a windows issue, where very high rotation
          // rates can cause the rename to fail with permission denied (because of antivirus?).
          details::os::sleep_for_millis(100);
          if(!rename_file_impl(src, target))
          {
            m_file_helper.reopen(true); // truncate the log file anyway to prevent it to grow beyond its limit!
            m_current_size = 0;
            throw_rexlog_ex("rotating_file_sink: failed renaming " + filename_to_str(src) + " to " + filename_to_str(target), errno);
          }
        }
      }
      m_file_helper.reopen(true);
    }

    // delete the target if exists, and rename the src file  to target
    // return true on success, false otherwise.
    template <typename Mutex>
    REXLOG_INLINE bool RotatingFileSink<Mutex>::rename_file_impl(const filename_t& srcFilename, const filename_t& targetFilename)
    {
      // try to delete the target file in case it already exists.
      (void)details::os::remove(targetFilename);
      return details::os::rename(srcFilename, targetFilename) == 0;
    }

  } // namespace sinks
} // namespace rexlog
