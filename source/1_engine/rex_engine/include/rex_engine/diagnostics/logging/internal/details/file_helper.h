

#pragma once

#include "rex_engine/diagnostics/logging/internal/common.h"

namespace rexlog
{
  namespace details
  {

    // Helper class for file sinks.
    // When failing to open a file, retry several times(5) with a delay interval(10 ms).
    // Throw rexlog_ex exception on errors.

    struct FilenameWithExtension
    {
      filename_t filename;
      filename_t ext;
    };

     class FileHelper
    {
    public:
      FileHelper() = default;
      explicit FileHelper(const FileEventHandlers& eventHandlers);

      FileHelper(const FileHelper&)            = delete;
      FileHelper& operator=(const FileHelper&) = delete;
      ~FileHelper();

      void open(const filename_t& fname, bool truncate = false);
      void reopen(bool truncate);
      void flush();
      void sync();
      void close();
      void write(const memory_buf_t& buf);
      size_t size() const;
      const filename_t& filename() const;

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
      static FilenameWithExtension split_by_extension(const filename_t& fname);

    private:
      static constexpr int s_open_tries             = 5;
      static constexpr unsigned int s_open_interval = 10;
      FILE* m_fd {nullptr};
      filename_t m_filename;
      FileEventHandlers m_event_handlers;
    };
  } // namespace details
} // namespace rexlog