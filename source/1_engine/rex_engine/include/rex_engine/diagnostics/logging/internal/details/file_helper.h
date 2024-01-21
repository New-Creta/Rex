

#pragma once

#include "rex_engine/diagnostics/logging/internal/common.h"
#include "rex_std/bonus/string.h"
#include "rex_std/functional.h"

#include <stdio.h>

namespace rex
{
  namespace log
  {
    struct FileEventHandlers
    {
      FileEventHandlers()
          : before_open(nullptr)
          , after_open(nullptr)
          , before_close(nullptr)
          , after_close(nullptr)
      {
      }

      rsl::function<void(const filename_t& filename)> before_open;
      rsl::function<void(const filename_t& filename, FILE* fileStream)> after_open;
      rsl::function<void(const filename_t& filename, FILE* fileStream)> before_close;
      rsl::function<void(const filename_t& filename)> after_close;
    };

    namespace details
    {
      // Helper class for file sinks.
      // When failing to open a file, retry several times(5) with a delay interval(10 ms).
      // Throw rexlog_ex exception on errors.

      struct FilenameWithExtension
      {
        rsl::small_stack_string filename;
        rsl::tiny_stack_string ext;
      };

      class FileHelper
      {
      public:
        FileHelper() = default;
        explicit FileHelper(const FileEventHandlers& eventHandlers);

        FileHelper(const FileHelper&)            = delete;
        FileHelper& operator=(const FileHelper&) = delete;
        ~FileHelper();

        void open(rsl::string_view fname, bool truncate = false);
        void reopen(bool truncate);
        void flush();
        void sync();
        void close();
        void write(const memory_buf_t& buf);
        size_t size() const;
        rsl::string_view filename() const;

        //
        // return file path and its extension:
        //
        // "mylog.txt" => ("mylog", ".txt")
        // "mylog" => ("mylog", "")
        // "mylog." => ("mylog.", "")
        // "/dir1/dir2/mylog.txt" => ("/dir1/dir2/mylog", ".txt")
        //
        // the starting dot in filenames is ignored (hidden files):
        //
        // ".mylog" => (".mylog". "")
        // "my_folder/.mylog" => ("my_folder/.mylog", "")
        // "my_folder/.mylog.txt" => ("my_folder/.mylog", ".txt")
        static FilenameWithExtension split_by_extension(rsl::string_view fname);

      private:
        static constexpr int s_open_tries             = 5;
        static constexpr unsigned int s_open_interval = 10;
        FILE* m_fd {nullptr};
        filename_t m_filename;
        FileEventHandlers m_event_handlers;
      };
    } // namespace details
  }   // namespace log
} // namespace rex