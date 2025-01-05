#pragma once

#include "rex_std/string_view.h"
#include "rex_std/memory.h"
#include "rex_std/bonus/memory.h"

namespace rex
{
	namespace vfs
	{
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
	}
}
