#pragma once

#include "rex_std/bonus/string.h"
#include "rex_std/vector.h"
#include "rex_std/mutex.h"
#include "rex_std/atomic.h"
#include "rex_std/string_view.h"
#include "rex_std/bonus/memory.h"

namespace rex
{
	namespace vfs
	{
    class ReadRequest;

    class QueuedRequest
    {
    public:
      explicit QueuedRequest(rsl::string_view filepath);

      void add_request_to_signal(ReadRequest* request);
      void remove_request_to_signal(ReadRequest* request);
      void swap_request_to_signal(ReadRequest* original, ReadRequest* newRequest);

      void signal_requests(const rsl::byte* buffer, rsl::memory_size size);

      bool all_requests_finished() const;

      rsl::string_view filepath() const;

    private:
      rsl::medium_stack_string m_filepath;
      rsl::vector<ReadRequest*> m_requests;
      rsl::mutex m_requests_access_mtx;
      rsl::atomic<bool> m_is_done;
    };

	}
}