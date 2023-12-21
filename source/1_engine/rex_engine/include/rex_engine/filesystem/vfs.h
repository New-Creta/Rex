#pragma once

#include "rex_engine/filesystem/mounting_point.h"
#include "rex_engine/memory/blob.h"
#include "rex_engine/types.h"
#include "rex_std/bonus/functional.h"
#include "rex_std/functional.h"
#include "rex_std/memory.h"
#include "rex_std/string_view.h"
#include "rex_std/type_traits.h"
#include "rex_std_extra/utility/type_id.h"
#include "rex_std_extra/utility/yes_no.h"

namespace rex
{
  namespace vfs
  {
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

    REX_NO_DISCARD memory::Blob open_read(rsl::string_view filepath);
    REX_NO_DISCARD memory::Blob open_read(MountingPoint root, rsl::string_view filepath);
    REX_NO_DISCARD ReadRequest open_read_async(rsl::string_view filepath);
    REX_NO_DISCARD ReadRequest open_read_async(MountingPoint root, rsl::string_view filepath);
    bool save_to_file(rsl::string_view filepath, const void* data, card64 size, AppendToFile shouldAppend);
    bool save_to_file(MountingPoint root, rsl::string_view filepath, const void* data, card64 size, AppendToFile shouldAppend);
    bool save_to_file(rsl::string_view filepath, const memory::Blob& blob, AppendToFile shouldAppend);
    bool save_to_file(MountingPoint root, rsl::string_view filepath, const memory::Blob& blob, AppendToFile shouldAppend);
    bool create_dir(rsl::string_view path);
    bool create_dir(MountingPoint root, rsl::string_view path);

    bool exists(rsl::string_view path);
    bool exists(MountingPoint root, rsl::string_view path);
    bool is_dir(rsl::string_view path);
    bool is_dir(MountingPoint root, rsl::string_view path);
    bool is_file(rsl::string_view path);
    bool is_file(MountingPoint root, rsl::string_view path);
    bool is_abs(rsl::string_view path);
    bool is_rel(rsl::string_view path);
    rsl::medium_stack_string create_full_path(rsl::string_view path);
    rsl::medium_stack_string create_full_path(MountingPoint root, rsl::string_view path);
  } // namespace vfs
} // namespace rex
