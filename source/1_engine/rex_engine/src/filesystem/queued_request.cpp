#include "rex_engine/filesystem/internal/queued_request.h"
#include "rex_engine/filesystem/read_request.h"

#include "rex_engine/diagnostics/assert.h"

namespace rex
{
	namespace vfs
	{
    QueuedRequest::QueuedRequest(rsl::string_view filepath)
      : m_filepath(filepath)
      , m_requests()
      , m_requests_access_mtx()
      , m_is_done(false)
    {
    }

    void QueuedRequest::add_request_to_signal(ReadRequest* request)
    {
      const rsl::unique_lock lock(m_requests_access_mtx);
      m_requests.push_back(request);
    }
    void QueuedRequest::remove_request_to_signal(ReadRequest* request)
    {
      const rsl::unique_lock lock(m_requests_access_mtx);
      auto it = rsl::find(m_requests.cbegin(), m_requests.cend(), request);

      if (it != m_requests.cend())
      {
        m_requests.erase(it);
      }
    }
    void QueuedRequest::swap_request_to_signal(ReadRequest* original, ReadRequest* newRequest)
    {
      const rsl::unique_lock lock(m_requests_access_mtx);
      auto it = rsl::find(m_requests.begin(), m_requests.end(), original);

      REX_ASSERT_X(it != m_requests.end(), "Request at {} not found as a request to signa", reinterpret_cast<void*>(original)); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)

      *it = newRequest;
    }

    void QueuedRequest::signal_requests(const rsl::byte* buffer, rsl::memory_size size)
    {
      const rsl::unique_lock lock(m_requests_access_mtx);

      for (ReadRequest* read_request : m_requests)
      {
        read_request->signal(buffer, size);
      }
    }

    bool QueuedRequest::all_requests_finished() const
    {
      return m_requests.empty();
    }

    rsl::string_view QueuedRequest::filepath() const
    {
      return m_filepath;
    }
	}
}