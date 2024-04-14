#pragma once

#include "rex_std/string.h"
#include "rex_engine/engine/types.h"
#include "rex_engine/memory/blob.h"

namespace rex
{
  // A file that's only in existance as long as the object is alive
  class TempFile
  {
  public:
    // the constructor will create the file
    explicit TempFile(rsl::string_view directory = "");
    TempFile(const TempFile&) = delete;
    TempFile(TempFile&&) = delete;
    // the destructor will delete the file
    ~TempFile();

    TempFile& operator=(const TempFile&) = delete;
    TempFile& operator=(TempFile&&) = delete;

    void write(const void* data, s32 size);
    memory::Blob read();

    rsl::string_view filename() const;

  private:
    rsl::string m_filename;
  };
}
