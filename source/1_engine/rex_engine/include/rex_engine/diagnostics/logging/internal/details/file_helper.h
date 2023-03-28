

#pragma once

#include <rexlog/common.h>

#include "rex_std/tuple.h"
#include <tuple>

namespace rexlog {
namespace details {

// Helper class for file sinks.
// When failing to open a file, retry several times(5) with a delay interval(10 ms).
// Throw rexlog_ex exception on errors.

  struct filename_with_extension
  {
    filename_t filename;
    filename_t ext;
  };

class REXLOG_API file_helper
{
public:
    file_helper() = default;
    explicit file_helper(const file_event_handlers &event_handlers);

    file_helper(const file_helper &) = delete;
    file_helper &operator=(const file_helper &) = delete;
    ~file_helper();

    void open(const filename_t &fname, bool truncate = false);
    void reopen(bool truncate);
    void flush();
    void sync();
    void close();
    void write(const memory_buf_t &buf);
    size_t size() const;
    const filename_t &filename() const;

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
    static filename_with_extension split_by_extension(const filename_t &fname);

private:
    const int open_tries_ = 5;
    const unsigned int open_interval_ = 10;
    FILE *fd_{nullptr};
    filename_t filename_;
    file_event_handlers event_handlers_;
};
} // namespace details
} // namespace rexlog