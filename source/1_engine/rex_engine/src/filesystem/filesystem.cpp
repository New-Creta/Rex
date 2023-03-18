#include "rex_engine/filesystem/filesystem.h"
#include "rex_engine/diagnostics/win/win_call.h"
#include "rex_std/bonus/string.h"
#include "rex_std/memory.h"
#include "rex_std_extra/memory.h"
#include "rex_engine/memory/untracker_allocator.h"

#include <Windows.h>

namespace rex::vfs
{
  rsl::medium_stack_string g_root;
  bool g_is_initialized = false;

  void init(rsl::string_view root)
  {
    if (root.empty())
    {
      WIN_CALL(GetCurrentDirectoryA(g_root.max_size(), g_root.data()));
      g_root.reset_null_termination_offset();
    }
    else
    {
      rsl::medium_stack_string cwd;
      WIN_CALL(GetCurrentDirectoryA(cwd.max_size(), cwd.data()));
      cwd.reset_null_termination_offset();

      g_root = cwd;
      g_root += "/";
      g_root += root;
    }

    g_is_initialized = true;
  }

  void save_to_file(rsl::string_view filePath, const void* data, card64 size)
  {
    REX_ASSERT_X(g_is_initialized, "Trying to use vfs before it's initialized");
  }
}