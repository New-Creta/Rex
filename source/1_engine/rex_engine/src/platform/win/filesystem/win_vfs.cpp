#include "rex_engine/platform/win/filesystem/win_vfs.h"

#include "rex_engine/cmdline/cmdline.h"
#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/diagnostics/logging/log_verbosity.h"
#include "rex_engine/engine/state_controller.h"
#include "rex_engine/engine/casting.h"
#include "rex_engine/filesystem/path.h"
#include "rex_engine/filesystem/file.h"
#include "rex_engine/filesystem/directory.h"
#include "rex_engine/platform/win/diagnostics/win_call.h"
#include "rex_std/bonus/atomic/atomic.h"
#include "rex_std/bonus/hashtable.h"
#include "rex_std/bonus/memory.h"
#include "rex_std/bonus/platform.h"
#include "rex_std/bonus/string.h"
#include "rex_std/bonus/utility/enum_reflection.h"
#include "rex_std/ctype.h"
#include "rex_std/mutex.h"
#include "rex_std/thread.h"
#include "rex_std/unordered_map.h"
#include "rex_std/vector.h"

#include <Windows.h>
#include <processenv.h>
#include <cstddef>

DEFINE_LOG_CATEGORY(LogFileSystem);

// NOLINTBEGIN(modernize-use-nullptr)

// For documentation on how the vfs works, see vfs.cpp

namespace rex
{
  namespace vfs
  {
    memory::Blob read_file(rsl::string_view filepath)
    {
      const rsl::string fullpath = create_full_path(filepath);
      return rex::file::read_file(fullpath);
    }

    Error save_to_file(rsl::string_view filepath, const void* data, card64 size, AppendToFile shouldAppend)
    {
      const rsl::string fullpath = create_full_path(filepath);
      if (shouldAppend)
      {
        return rex::file::append_text(fullpath, rsl::string_view((const char8*)data, narrow_cast<s32>(size)));
      }
      else
      {
        return rex::file::save_to_file(fullpath, data, size);
      }
    }

    Error create_dir(rsl::string_view path)
    {
      const rsl::string fullpath = create_full_path(path);
      return directory::create(fullpath);
    }

    Error create_dirs(rsl::string_view path)
    {
      const rsl::string fullpath = create_full_path(path);
      return directory::create_recursive(fullpath);
    }

  } // namespace vfs
} // namespace rex

// NOLINTEND(modernize-use-nullptr)
