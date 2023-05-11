#pragma once

#include "rex_engine/types.h"

#include "rex_std/type_traits.h"
#include "rex_std/functional.h"
#include "rex_std/memory.h"
#include "rex_std/string_view.h"
#include "rex_std/bonus/functional.h"
#include "rex_std_extra/utility/type_id.h"
#include "rex_std_extra/utility/yes_no.h"

namespace rex::vfs
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

    void signal(const char8* buffer, count_t count);
    void wait();

    const char8* data() const;
    count_t count() const;

    rsl::string_view filepath() const;

  private:
    rsl::string_view m_filepath;
    QueuedRequest* m_queued_request;
    bool m_is_done;
    const char8* m_buffer;
    count_t m_count;
  };

  void init(rsl::string_view root = "");
  void shutdown();

  REX_NO_DISCARD rsl::unique_array<char8> open_read(rsl::string_view filepath);
  REX_NO_DISCARD ReadRequest open_read_async(rsl::string_view filepath);
  void save_to_file(rsl::string_view filepath, const void* data, card64 size, AppendToFile shouldAppend);
  void create_dir(rsl::string_view path);

  bool exists(rsl::string_view path);
  bool is_dir(rsl::string_view path);
  bool is_file(rsl::string_view path);
  bool is_abs(rsl::string_view path);
  bool is_rel(rsl::string_view path);
  rsl::medium_stack_string create_full_path(rsl::string_view path);
}
