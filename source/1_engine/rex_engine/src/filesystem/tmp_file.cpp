#include "rex_engine/filesystem/tmp_file.h"

#include "rex_engine/filesystem/path.h"
#include "rex_engine/filesystem/file.h"

namespace rex
{
  TempFile::TempFile(rsl::string_view directory)
  {
    m_filepath = path::abs_path(path::join(directory, path::random_filename()));
    file::create(m_filepath);
  }

  TempFile::~TempFile()
  {
    file::del(m_filepath);
  }

  void TempFile::write(const void* data, s32 size)
  {
    file::append_text(m_filepath, rsl::string_view((const char8*)data, size));
  }

  memory::Blob TempFile::read()
  {
    return file::read_file(m_filepath);
  }

  rsl::string_view TempFile::filename() const
  {
    return rex::path::filename(m_filepath);
  }

  rsl::string_view TempFile::filepath() const
  {
    return m_filepath;
  }
}