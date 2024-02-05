#include "rex_engine/platform/win/filesystem/win_vfs.h"

#include "rex_engine/cmdline/cmdline.h"
#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/diagnostics/logging/log_verbosity.h"
#include "rex_engine/engine/state_controller.h"
#include "rex_engine/filesystem/path.h"
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
#include <stddef.h>

DEFINE_LOG_CATEGORY(FileSystem, rex::LogVerbosity::Log);

// NOLINTBEGIN(modernize-use-nullptr)

// For documentation on how the vfs works, see vfs.cpp

namespace rex
{
  namespace vfs
  {
    memory::Blob read_file(rsl::string_view filepath)
    {
      rsl::string path = create_full_path(filepath);

      const rsl::win::handle handle(WIN_CALL_IGNORE(CreateFileA(path.data(),               // Path to file
                                                                GENERIC_READ,              // General read and write access
                                                                FILE_SHARE_READ,           // Other processes can also read the file
                                                                NULL,                      // No SECURITY_ATTRIBUTES
                                                                OPEN_EXISTING,             // Open the file, only if it exists
                                                                FILE_FLAG_SEQUENTIAL_SCAN, // Files will be read from beginning to end
                                                                NULL                       // No template file
                                                                ),
                                                    ERROR_ALREADY_EXISTS));

      // prepare a buffer to receive the file content
      const card32 file_size              = static_cast<card32>(GetFileSize(handle.get(), nullptr));
      rsl::unique_array<rsl::byte> buffer = rsl::make_unique<rsl::byte[]>(file_size + 1); // NOLINT(modernize-avoid-c-arrays)
      buffer[file_size]                   = static_cast<rsl::byte>(0);                    // make sure we end with a null char

      // actually read the file
      DWORD bytes_read = 0;
      WIN_CALL(ReadFile(handle.get(), buffer.get(), static_cast<DWORD>(buffer.count()), &bytes_read, NULL));

      // return the buffer
      return memory::Blob(rsl::move(buffer));
    }

    bool save_to_file(rsl::string_view filepath, const void* data, card64 size, AppendToFile shouldAppend)
    {
      rsl::string fullpath = create_full_path(filepath);

      const rsl::win::handle handle(WIN_CALL_IGNORE(CreateFileA(fullpath.data(),           // Path to file
                                                                GENERIC_WRITE,             // General read and write access
                                                                FILE_SHARE_READ,           // Other processes can also read the file
                                                                NULL,                      // No SECURITY_ATTRIBUTES
                                                                OPEN_ALWAYS,               // Create a new file, error when it already exists
                                                                FILE_FLAG_SEQUENTIAL_SCAN, // Files will be read from beginning to end
                                                                NULL                       // No template file
                                                                ),
                                                    ERROR_ALREADY_EXISTS));

      const DWORD move_method = shouldAppend ? FILE_END : FILE_BEGIN;

      // either trunc or append to the file
      WIN_CALL(SetFilePointer(handle.get(), 0, NULL, move_method));
      WIN_CALL(SetEndOfFile(handle.get()));

      DWORD bytes_written = 0;
      return WIN_SUCCESS(WriteFile(handle.get(), data, static_cast<DWORD>(size), &bytes_written, NULL));
    }

    bool create_dir(rsl::string_view path)
    {
      rsl::string fullpath = create_full_path(path);
      return WIN_SUCCESS_IGNORE(CreateDirectoryA(fullpath.data(), NULL), ERROR_ALREADY_EXISTS);
    }

  } // namespace vfs
} // namespace rex

// NOLINTEND(modernize-use-nullptr)
