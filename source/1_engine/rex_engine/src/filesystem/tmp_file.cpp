#include "rex_engine/filesystem/tmp_file.h"

#include "rex_engine/filesystem/path.h"
#include "rex_engine/filesystem/file.h"

namespace rex
{
  TempFile::TempFile(rsl::string_view directory)
  {
    m_filename = path::abs_path(path::join(directory, path::random_filename()));
    file::create(m_filename);
  }

  TempFile::~TempFile()
  {
    file::del(m_filename);
  }

  void TempFile::write(const void* data, s32 size)
  {
    file::append_text(m_filename, rsl::string_view((const char8*)data, size));
  }

  rsl::string_view TempFile::filename() const
  {
    return m_filename;
  }

}