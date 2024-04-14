#include "rex_engine/platform/win/filesystem/win_file.h"

#include "rex_engine/engine/numeric.h"
#include "rex_engine/diagnostics/log.h"
#include "rex_engine/filesystem/path.h"
#include "rex_engine/platform/win/diagnostics/win_call.h"
#include "rex_std/bonus/platform.h"
#include "rex_std/bonus/time/win/win_time_functions.h"

#include <Windows.h>

// NOLINTBEGIN(modernize-use-nullptr)

namespace rex
{
  namespace file
  {
    DEFINE_LOG_CATEGORY(LogFile);

    namespace internal
    {
      void append_to_file(HANDLE handle)
      {
        SetFilePointer(handle, 0, NULL, FILE_END);
        SetEndOfFile(handle);
      }

      void trunc_file(HANDLE handle)
      {
        SetFilePointer(handle, 0, NULL, FILE_BEGIN);
        SetEndOfFile(handle);
      }

      rsl::win::handle open_file_for_attribs(rsl::string_view path)
      {
        rsl::win::handle file(CreateFileA(path.data(), FILE_READ_ATTRIBUTES, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, nullptr));

        return file;
      }
    } // namespace internal

    // Read from a file
    memory::Blob read_file(rsl::string_view path)
    {
      const rsl::string full_path = path::abs_path(path);

      const rsl::win::handle handle(WIN_CALL_IGNORE(CreateFileA(full_path.data(),               // Path to file
        GENERIC_READ,              // General read and write access
        FILE_SHARE_READ,           // Other processes can also read the file
        NULL,                      // No SECURITY_ATTRIBUTES
        OPEN_EXISTING,             // Open the file, only if it exists
        FILE_FLAG_SEQUENTIAL_SCAN, // Files will be read from beginning to end
        NULL                       // No template file
      ),
        ERROR_ALREADY_EXISTS));

      if (!handle.is_valid())
      {
        REX_ERROR(LogFile, "Failed to open file {}", full_path);
        return {};
      }

      // prepare a buffer to receive the file content
      const card32 file_size = static_cast<card32>(GetFileSize(handle.get(), nullptr));
      rsl::unique_array<rsl::byte> buffer = rsl::make_unique<rsl::byte[]>(file_size + 1); // NOLINT(modernize-aError-c-arrays)
      buffer[file_size] = static_cast<rsl::byte>(0);                    // make sure we end with a null char

      // actually read the file
      DWORD bytes_read = 0;
      WIN_CALL(ReadFile(handle.get(), buffer.get(), static_cast<DWORD>(buffer.count()), &bytes_read, NULL));

      // return the buffer
      return memory::Blob(rsl::move(buffer));
    }
    // Save content to a file
    Error save_to_file(rsl::string_view filepath, const void* data, card64 size)
    {
      const rsl::string fullpath = path::abs_path(filepath);

      const rsl::win::handle handle(WIN_CALL_IGNORE(CreateFileA(fullpath.data(),           // Path to file
        GENERIC_WRITE,             // General read and write access
        FILE_SHARE_READ,           // Other processes can also read the file
        NULL,                      // No SECURITY_ATTRIBUTES
        OPEN_ALWAYS,               // Create a new file, error when it already exists
        FILE_FLAG_SEQUENTIAL_SCAN, // Files will be read from beginning to end
        NULL                       // No template file
      ), ERROR_ALREADY_EXISTS));

      // make sure the handle is valid
      if (!handle.is_valid())
      {
        return Error::create_with_log(LogFile, "Failed to open file at \"{}\"", fullpath);
      }

      // either trunc or append to the file
      WIN_CALL(SetFilePointer(handle.get(), 0, NULL, FILE_BEGIN));
      WIN_CALL(SetEndOfFile(handle.get()));

      DWORD bytes_written = 0;
      const bool success = WIN_SUCCESS(WriteFile(handle.get(), data, static_cast<DWORD>(size), &bytes_written, NULL));

      return success
        ? Error::no_error()
        : Error::create_with_log(LogFile, "Failed to write to \"{}\"", fullpath);
    }
    // Append lines to a file
    Error append_lines(rsl::string_view path, const rsl::vector<rsl::string>& lines)
    {
      const rsl::string full_path = path::abs_path(path);

      if(is_readonly(full_path))
      {
        return Error::create_with_log(LogFile, "File \"\" is read only. Cannot append lines", full_path);
      }

      const rsl::win::handle handle(WIN_CALL_IGNORE(CreateFileA(full_path.c_str(),             // Path to file
                                                                GENERIC_READ | GENERIC_WRITE,  // General read and write access
                                                                FILE_SHARE_READ,               // Other processes can also read the file
                                                                NULL,                          // No SECURITY_ATTRIBUTES
                                                                OPEN_EXISTING,                 // Open the file, only if it exists
                                                                FILE_FLAG_SEQUENTIAL_SCAN,     // Files will be read from beginning to end
                                                                NULL                           // No template file
                                                                ),
                                                    ERROR_ALREADY_EXISTS));

      if (!handle.is_valid())
      {
        return Error::create_with_log(LogFile, "Cannot append to file \"{}\" as it doesn't exist", full_path);
      }

      internal::append_to_file(handle.get());

      for(const rsl::string_view line: lines)
      {
        WIN_CALL(WriteFile(handle.get(), line.data(), line.length(), NULL, NULL));
      }

      return Error::no_error();
    }
    // Append text to a file
    Error append_text(rsl::string_view path, rsl::string_view txt)
    {
      const rsl::string full_path = path::abs_path(path);

      if(is_readonly(full_path))
      {
        return Error::create_with_log(LogFile, "File \"{}\" is read only. Cannot append lines", full_path);
      }

      const rsl::win::handle handle(WIN_CALL_IGNORE(CreateFileA(full_path.c_str(),            // Path to file
                                                                GENERIC_READ | GENERIC_WRITE, // General read and write access
                                                                FILE_SHARE_READ,              // Other processes can also read the file
                                                                NULL,                         // No SECURITY_ATTRIBUTES
                                                                OPEN_EXISTING,                // Open the file, only if it exists
                                                                FILE_FLAG_SEQUENTIAL_SCAN,    // Files will be read from beginning to end
                                                                NULL                          // No template file
                                                                ),
                                                    ERROR_ALREADY_EXISTS));

      if (!handle.is_valid())
      {
        return Error::create_with_log(LogFile, "Cannot append to file \"{}\" as it doesn't exist", full_path);
      }

      internal::append_to_file(handle.get());

      WriteFile(handle.get(), txt.data(), txt.length(), NULL, NULL);

      return Error::no_error();
    }
    // Trunc a file, removing all content
    Error trunc(rsl::string_view path)
    {
      const rsl::string full_path = path::abs_path(path);

      if(is_readonly(full_path))
      {
        return Error::create_with_log(LogFile, "File \"{}\" is read only. Cannot append lines", full_path);
      }

      const rsl::win::handle handle(WIN_CALL_IGNORE(CreateFileA(full_path.c_str(),              // Path to file
                                                                GENERIC_READ | GENERIC_WRITE,   // General read and write access
                                                                FILE_SHARE_READ,                // Other processes can also read the file
                                                                NULL,                           // No SECURITY_ATTRIBUTES
                                                                OPEN_EXISTING,                  // Open the file, only if it exists
                                                                FILE_FLAG_SEQUENTIAL_SCAN,      // Files will be read from beginning to end
                                                                NULL                            // No template file
                                                                ),
                                                    ERROR_ALREADY_EXISTS));

      if (!handle.is_valid())
      {
        return Error::create_with_log(LogFile, "Cannot trunc file \"{}\" as it doesn't exist", full_path);
      }

      internal::trunc_file(handle.get());

      return Error::no_error();
    }
    // Copy a file, overwiting an existing one is possible
    Error copy(rsl::string_view src, rsl::string_view dst, OverwriteIfExist overwriteIfExist)
    {
      const rsl::string full_src = path::abs_path(src);
      const rsl::string full_dst = path::abs_path(dst);
      const bool success = WIN_SUCCESS(CopyFileA(full_src.c_str(), full_dst.c_str(), !overwriteIfExist)); // NOLINT(readability-implicit-bool-conversion)
      return success
        ? Error::no_error()
        : Error::create_with_log(LogFile, "cannot copy \"{}\" to \"{}\"", full_src, full_dst);
    }
    // Move/Rename a file, overwriting an existing one is possible
    Error move(rsl::string_view src, rsl::string_view dst)
    {
      const rsl::string full_src = path::abs_path(src);
      const rsl::string full_dst = path::abs_path(dst);

      if(file::exists(full_dst))
      {
        return Error::create_with_log(LogFile, "Cannot move to \"{}\", file already exists", dst);
      }

      const bool success = WIN_SUCCESS(MoveFileA(full_src.c_str(), full_dst.c_str()));

      return success
        ? Error::no_error()
        : Error::create_with_log(LogFile, "cannot move \"{}\" to \"{}\"", full_src, full_dst);
    }
    // Create a new empty file
    Error create(rsl::string_view path)
    {
      const rsl::string full_path = path::abs_path(path);

      if(file::exists(full_path))
      {
        return Error::create_with_log(LogFile, "cannot create file at \"{}\" as it already exists", full_path);
      }

      const rsl::win::handle handle(WIN_CALL_IGNORE(CreateFileA(full_path.c_str(),         // Path to file
                                                                GENERIC_WRITE,             // General read and write access
                                                                FILE_SHARE_READ,           // Other processes can also read the file
                                                                NULL,                      // No SECURITY_ATTRIBUTES
                                                                CREATE_NEW,                // Open the file, only if it exists
                                                                FILE_FLAG_SEQUENTIAL_SCAN, // Files will be read from beginning to end
                                                                NULL                       // No template file
                                                                ),
                                                    ERROR_FILE_NOT_FOUND));

      return handle.is_valid()
        ? Error::no_error()
        : Error::create_with_log(LogFile, "Failed to create file at \"{}\"", full_path);
    }
    // Delete a file
    Error del(rsl::string_view path)
    {
      const rsl::string full_path = path::abs_path(path);

      if(!file::exists(full_path))
      {
        return Error::create_with_log(LogFile, "cannot delete file at \"{}\" as it doesn't exist", full_path);
      }

      const bool success = WIN_SUCCESS(DeleteFileA(full_path.c_str()));
      return success
        ? Error::no_error()
        : Error::create_with_log(LogFile, "Failed to delete file at \"{}\"", full_path);
    }
    // return the file size
    card64 size(rsl::string_view path)
    {
      const rsl::string full_path       = path::abs_path(path);
      const rsl::win::handle file = internal::open_file_for_attribs(full_path);

      // When we have an invalid handle, we simply return the input
      if(!file.is_valid())
      {
        return -1;
      }

      DWORD high_word      = 0;
      DWORD const low_word = GetFileSize(file.get(), &high_word);
      return static_cast<card64>(rex::merge_int32_to_int64(high_word, low_word));
    }
    // Check if a file exists
    bool exists(rsl::string_view path)
    {
      const rsl::string full_path = path::abs_path(path);
      const DWORD attribs   = WIN_CALL_IGNORE(GetFileAttributesA(full_path.c_str()), ERROR_FILE_NOT_FOUND);

      if(attribs == INVALID_FILE_ATTRIBUTES)
      {
        return false;
      }

      if((attribs & FILE_ATTRIBUTE_DIRECTORY) != 0u)
      {
        return false;
      }

      return true;
    }
    // Check if a file is marked read only
    bool is_readonly(rsl::string_view path)
    {
      const rsl::string full_path = path::abs_path(path);
      if(!exists(full_path))
      {
        return false;
      }

      const DWORD attribs = GetFileAttributesA(full_path.c_str());
      return attribs & FILE_ATTRIBUTE_READONLY; // NOLINT(readability-implicit-bool-conversion)
    }
    // Set a file to be readonly
    Error set_readonly(rsl::string_view path)
    {
      const rsl::string full_path = path::abs_path(path);
      if(!exists(full_path))
      {
        return Error::create_with_log(LogFile, "Can't set readonly attribute for \"{}\" as the file doesn't exist", full_path);
      }

      DWORD attribs = GetFileAttributesA(full_path.c_str());
      attribs |= FILE_ATTRIBUTE_READONLY;
      SetFileAttributesA(full_path.c_str(), attribs);

      return Error::no_error();
    }
    // Remove the readonly flag of a file
    Error remove_readonly(rsl::string_view path)
    {
      const rsl::string full_path = path::abs_path(path);
      if(!exists(full_path))
      {
        return Error::create_with_log(LogFile, "Can't remove readonly attribute for \"{}\" as the file doesn't exist", full_path);
      }

      DWORD attribs = GetFileAttributesA(full_path.c_str());
      attribs &= ~FILE_ATTRIBUTE_READONLY; // NOLINT(hicpp-signed-bitwise)
      SetFileAttributesA(full_path.c_str(), attribs);

      return Error::no_error();
    }
    // Return the creation time of a file
    rsl::time_point creation_time(rsl::string_view path)
    {
      const rsl::win::handle file = internal::open_file_for_attribs(path);

      // When we have an invalid handle, we return 0
      if(!file.is_valid())
      {
        return rsl::time_point();
      }

      FILETIME creation_time {};
      GetFileTime(file.get(), &creation_time, nullptr, nullptr);
      const SYSTEMTIME sys_time = rsl::win::to_local_sys_time(creation_time);
      return rsl::timepoint_from_systime(sys_time);
    }
    // Return the access time of a file
    rsl::time_point access_time(rsl::string_view path)
    {
      const rsl::win::handle file = internal::open_file_for_attribs(path);

      // When we have an invalid handle, we return 0
      if(!file.is_valid())
      {
        return rsl::time_point();
      }

      FILETIME access_time {};
      GetFileTime(file.get(), nullptr, &access_time, nullptr);
      const SYSTEMTIME sys_time = rsl::win::to_local_sys_time(access_time);
      return rsl::timepoint_from_systime(sys_time);
    }
    // Return the modification time of a file
    rsl::time_point modification_time(rsl::string_view path)
    {
      const rsl::win::handle file = internal::open_file_for_attribs(path);

      // When we have an invalid handle, we return 0
      if(!file.is_valid())
      {
        return rsl::time_point();
      }

      FILETIME modification_time {};
      GetFileTime(file.get(), nullptr, nullptr, &modification_time);
      const SYSTEMTIME sys_time = rsl::win::to_local_sys_time(modification_time);
      return rsl::timepoint_from_systime(sys_time);
    }
  } // namespace file
} // namespace rex

// NOLINTEND(modernize-use-nullptr)
