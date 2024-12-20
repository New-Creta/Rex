#include "rex_engine/filesystem/tmp_dir.h"

#include "rex_engine/filesystem/path.h"
#include "rex_engine/filesystem/directory.h"
#include "rex_engine/filesystem/file.h"

namespace rex
{
  TempDirectory::TempDirectory(rsl::string_view directory)
  {
    m_dirname = path::abs_path(path::join(directory, path::random_dir()));
    directory::create(m_dirname);
  }

  TempDirectory::~TempDirectory()
  {
    directory::del_recursive(m_dirname);
  }

  Error TempDirectory::create_file(rsl::string_view filename)
  {
    if (rex::path::is_absolute(filename) && !rex::path::is_under_dir(filename, m_dirname))
    {
      return Error(rsl::format("\"{}\" is not under temp directory \"{}\". Abort file creation", filename, m_dirname));
    }

    return file::create(path::join(m_dirname, filename));
  }
  Error TempDirectory::create_dir(rsl::string_view dirname)
  {
    if (rex::path::is_absolute(dirname) && !rex::path::is_under_dir(dirname, m_dirname))
    {
      return Error(rsl::format("\"{}\" is not under temp directory \"{}\". Abort file creation", dirname, m_dirname));
    }

    return directory::create(path::join(m_dirname, dirname));
  }

  rsl::string_view TempDirectory::dirname() const
  {
    return m_dirname;
  }

}