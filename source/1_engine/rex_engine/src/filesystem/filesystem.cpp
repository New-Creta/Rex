#include "rex_engine/filesystem/filesystem.h"
#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/diagnostics/win/win_call.h"
#include "rex_engine/memory/untracked_allocator.h"
#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_std/bonus/string.h"
#include "rex_std/bonus/platform/windows/handle.h"
#include "rex_std/memory.h"
#include "rex_std_extra/memory.h"

#include <Windows.h>

DEFINE_LOG_CATEGORY(RexFileSystem, rex::LogVerbosity::Log);

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
    else if (is_abs(root))
    {
      g_root = root;
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

    REX_ASSERT_X(is_dir(g_root), "root of vfs is not a directory");

    REX_LOG(RexFileSystem, "FileSystem initialised with root '{}'", g_root);

    g_is_initialized = true;
  }

  rsl::unique_array<char8> open_file(rsl::string_view filepath)
  {
    REX_ASSERT_X(g_is_initialized, "Trying to use vfs before it's initialized");

    rsl::medium_stack_string path = g_root;
    path += "/";
    path += filepath;

    rsl::win::handle handle(WIN_CALL_IGNORE(CreateFile(
      path.data(),				      // Path to file
      GENERIC_READ | GENERIC_WRITE,	// General read and write access
      FILE_SHARE_READ,				      // Other processes can also read the file
      NULL,							            // No SECURITY_ATTRIBUTES 
      OPEN_ALWAYS,						      // Always open the file, create it if it doesn't exist
      FILE_FLAG_SEQUENTIAL_SCAN,		// Files will be read from beginning to end
      NULL							            // No template file
    ), ERROR_ALREADY_EXISTS));

    DWORD file_size = GetFileSize(handle.get(), nullptr);

    rsl::unique_array<char8> buffer = rsl::make_unique<char8[]>(file_size);

    DWORD bytes_read = 0;
    WIN_CALL(ReadFile(handle.get(), buffer.get(), static_cast<DWORD>(buffer.count()), &bytes_read, NULL));

    return buffer;
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

  bool exists(rsl::string_view path)
  {
    DWORD ftyp = GetFileAttributesA(path.data());
    if (ftyp == INVALID_FILE_ATTRIBUTES)
      return false;

    return true;
  }

  bool is_dir(rsl::string_view path)
  {
    DWORD ftyp = GetFileAttributesA(path.data());
    if (ftyp == INVALID_FILE_ATTRIBUTES)
      return false;

    if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
      return true;

    return false;
  }
  bool is_file(rsl::string_view path)
  {
    DWORD ftyp = GetFileAttributesA(path.data());
    if (ftyp == INVALID_FILE_ATTRIBUTES)
      return false;

    if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
      return false;

    return true;
  }
  bool is_abs(rsl::string_view path)
  {
    if (path.length() < 2)
      return false;

    if (!rsl::is_alpha(path[0]))
      return false;

    if (path[1] != ':')
      return false;

    if (path[2] != '/' && path[2] != '\\')
      return false;

    return true;
  }
  bool is_rel(rsl::string_view path)
  {
    return !is_abs(path);
  }
}