#pragma once

#include "rex_std/string.h"
#include "rex_std/string_view.h"
#include "rex_engine/engine/types.h"
#include "rex_engine/diagnostics/error.h"

namespace rex
{
  // A directory that's only in existance as long as the object is alive
  class TempDirectory
  {
  public:
    // the constructor will create the file
    explicit TempDirectory(rsl::string_view directory = "");
    TempDirectory(const TempDirectory&) = delete;
    TempDirectory(TempDirectory&&) = delete;
    // the destructor will delete the file
    ~TempDirectory();

    TempDirectory& operator=(const TempDirectory&) = delete;
    TempDirectory& operator=(TempDirectory&&) = delete;

    // Create a file under this temp directory
    Error create_file(rsl::string_view filename);
    // Create a directory under this temp directory
    Error create_dir(rsl::string_view dirname);

    rsl::string_view dirname() const;

  private:
    rsl::string m_dirname;
  };
}
