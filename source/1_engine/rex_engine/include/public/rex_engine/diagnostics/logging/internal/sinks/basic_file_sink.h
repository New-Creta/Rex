

#pragma once

#include "rex_engine/diagnostics/logging/internal/common.h"
#include "rex_engine/diagnostics/logging/internal/details/file_helper.h"
#include "rex_engine/diagnostics/logging/internal/details/null_mutex.h"
#include "rex_engine/diagnostics/logging/internal/sinks/base_sink.h"
#include "rex_engine/filesystem/file.h"
#include "rex_engine/filesystem/vfs.h"

#include <mutex>
#include <string>

namespace rex
{
  namespace log
  {
    namespace sinks
    {
      /*
       * Trivial file sink with single file as target
       */
      template <typename Mutex>
      class BasicFileSink final : public BaseSink<Mutex>
      {
      public:
        explicit BasicFileSink(rsl::string_view filename, bool truncate = false, const FileEventHandlers& eventHandlers = {});
        rsl::string_view filename() const;

      protected:
        void sink_it_impl(const details::LogMsg& msg) override;
        void flush_it_impl() override;

      private:
        details::FileHelper m_file_helper;
        debug_string m_filename;
      };

      template <typename Mutex>
      BasicFileSink<Mutex>::BasicFileSink(rsl::string_view filename, bool /*truncate*/, const FileEventHandlers& eventHandlers)
          : m_file_helper {eventHandlers}
          , m_filename(filename)
      {
        /*if (file::exists(filename))
        {
          return;
        }
        m_file_helper.open(filename, truncate);*/
      }

      template <typename Mutex>
      rsl::string_view BasicFileSink<Mutex>::filename() const
      {
        return m_file_helper.filename();
      }

      template <typename Mutex>
      void BasicFileSink<Mutex>::sink_it_impl(const details::LogMsg& msg)
      {
        memory_buf_t formatted;
        BaseSink<Mutex>::formatter().format(msg, formatted);
        const s32 msg_size = formatted.size();
        const auto* data   = formatted.data();

        vfs::write_to_file(m_filename, data, msg_size, vfs::AppendToFile::yes);
      }

      template <typename Mutex>
      void BasicFileSink<Mutex>::flush_it_impl()
      {
        // m_file_helper.flush();
      }

      using basic_file_sink_mt = BasicFileSink<rsl::mutex>;
      using basic_file_sink_st = BasicFileSink<details::NullMutex>;

    } // namespace sinks
  }   // namespace log
} // namespace rex