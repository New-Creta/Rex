#pragma once

#include "rex_engine/filesystem/mounting_point.h"
#include "rex_engine/memory/blob.h"
#include "rex_std/bonus/attributes.h"
#include "rex_std/bonus/memory/memory_size.h"
#include "rex_std/bonus/string.h"
#include "rex_std/bonus/types.h"
#include "rex_std/bonus/utility/yes_no.h"
#include "rex_std/string_view.h"
#include "rex_engine/engine/defines.h"

/// Rex VFS system supports syncronous and async file IO
/// We want to go for very simplistic design API at the moment
/// we may want to change this in the future if it's needed
///
/// SYNCRONOUS FILE IO
///
/// File Reading:
// rex::memory::Blob content = rex::vfs::read_file("path/to/file");
///
/// File Writing:
// int x = 0;
// rex::vfs::write_to_file("path/to/file.txt", &x, sizeof(x);
///
/// ASYNC FILE IO
///
/// We only support async file reading, as async file writing
/// Doesn't make much sense for the moment
///
// rex::vfs::ReadRequest read_request = rex::vfs::read_file_async("path/to/file");
///
/// Do some code ..
///
/// Wait for the request to finish reading its data
// read_request.wait();
///
// const rsl::byte* content = read_request.buffer();
// rsl::memory_size size = read_request.size();
///
/// Do something with the content
/// ..

namespace rex
{
  namespace vfs
  {
    // Returns the root of all files
    rsl::string_view root();

    // Returns the root directory of the engine files
    rsl::string_view engine_root();

    // Returns the root directory of the editor files
    rsl::string_view editor_root();

    // Returns the root directory of the current project
    rsl::string_view project_root();

    // Returns the root for all sessions data
    rsl::string_view sessions_root();

    // Returns the root for all sessions data of this project
    rsl::string_view project_sessions_root();

    // Returns the root for all files outputed during this session run (eg. logs)
    rsl::string_view session_data_root();

    DEFINE_YES_NO_ENUM(AppendToFile);

    class QueuedRequest;

    class ReadRequest
    {
    public:
      ReadRequest(rsl::string_view filepath, QueuedRequest* queuedRequest);
      ReadRequest(const ReadRequest& other);
      ReadRequest(ReadRequest&& other);

      ~ReadRequest();

      ReadRequest& operator=(const ReadRequest& other);
      ReadRequest& operator=(ReadRequest&& other);

      void signal(const rsl::byte* buffer, rsl::memory_size size);
      void wait() const;

      const rsl::byte* data() const;
      rsl::memory_size count() const;

      rsl::string_view filepath() const;

    private:
      rsl::string_view m_filepath;
      QueuedRequest* m_queued_request;
      bool m_is_done;
      const rsl::byte* m_buffer;
      rsl::memory_size m_size;
    };

    void init(rsl::string_view root = "");
    void set_root(rsl::string_view root);
    void mount(MountingPoint root, rsl::string_view path);
    void shutdown();

    REX_NO_DISCARD memory::Blob read_file(MountingPoint root, rsl::string_view filepath);
    REX_NO_DISCARD ReadRequest read_file_async(rsl::string_view filepath);
    REX_NO_DISCARD ReadRequest read_file_async(MountingPoint root, rsl::string_view filepath);
    bool save_to_file(MountingPoint root, rsl::string_view filepath, const void* data, card64 size, AppendToFile shouldAppend);
    bool save_to_file(rsl::string_view filepath, const memory::Blob& blob, AppendToFile shouldAppend);
    bool save_to_file(MountingPoint root, rsl::string_view filepath, const memory::Blob& blob, AppendToFile shouldAppend);
    bool create_dir(MountingPoint root, rsl::string_view path);
    bool create_dirs(rsl::string_view path);

    bool exists(MountingPoint root, rsl::string_view path);
    bool is_dir(MountingPoint root, rsl::string_view path);
    bool is_file(MountingPoint root, rsl::string_view path);
    rsl::string create_full_path(rsl::string_view path);
    rsl::string create_full_path(MountingPoint root, rsl::string_view path);

    rsl::string_view mount_path(MountingPoint mount);
    rsl::string_view no_mount_path();

    rsl::vector<rsl::string> files_in_dir(rsl::string_view path);

    // **************************************************************************
    // PLATFORM SPECIFIC IMPLEMENTATIONS
    // **************************************************************************
    REX_NO_DISCARD memory::Blob read_file(rsl::string_view filepath);
    bool save_to_file(rsl::string_view filepath, const void* data, card64 size, AppendToFile shouldAppend);
    bool create_dir(rsl::string_view path);

  }
}

#ifdef REX_PLATFORM_WINDOWS
  #include "rex_engine/platform/win/filesystem/win_vfs.h"
#endif