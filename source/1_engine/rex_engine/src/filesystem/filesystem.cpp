#include "rex_engine/filesystem/filesystem.h"
#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/diagnostics/win/win_call.h"
#include "rex_engine/memory/untracked_allocator.h"
#include "rex_std/bonus/string.h"
#include "rex_std/bonus/platform/windows/handle.h"
#include "rex_std/memory.h"
#include "rex_std_extra/memory.h"

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

  void save_to_file(rsl::string_view filePath, const void* data, card64 size, bool append)
  {
    REX_ASSERT_X(g_is_initialized, "Trying to use vfs before it's initialized");

    rsl::win::handle handle(WIN_CALL(CreateFile(
      filePath.data(),				      // Path to file
      GENERIC_READ | GENERIC_WRITE,	// General read and write access
      FILE_SHARE_READ,				      // Other processes can also read the file
      NULL,							            // No SECURITY_ATTRIBUTES 
      OPEN_ALWAYS,						      // Create a new file, error when it already exists
      FILE_FLAG_SEQUENTIAL_SCAN,		// Files will be read from beginning to end
      NULL							            // No template file
    )));

    if (append)
    {
      WIN_CALL(SetFilePointer(handle.get(), 0, NULL, FILE_END));
      WIN_CALL(SetEndOfFile(handle.get()));
    }

    DWORD bytes_written = 0;
    WIN_CALL(WriteFile(handle.get(), data, static_cast<DWORD>(size), &bytes_written, NULL));
  }
}