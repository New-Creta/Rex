#include "rex_engine/filesystem/read_request.h"

#include "rex_engine/filesystem/internal/queued_request.h"
#include "rex_engine/diagnostics/assert.h"

#include "rex_std/chrono.h"
#include "rex_std/thread.h"

namespace rex
{
	namespace vfs
	{
    ReadRequest::ReadRequest(rsl::string_view filepath, QueuedRequest* queuedRequest)
      : m_filepath(filepath)
      , m_queued_request(queuedRequest)
      , m_is_done(false)
      , m_buffer(nullptr)
      , m_size(0_bytes)
    {
    }

    ReadRequest::ReadRequest(const ReadRequest& other)
      : m_filepath(other.m_filepath)
      , m_queued_request(other.m_queued_request)
      , m_is_done(other.m_is_done)
      , m_buffer(other.m_buffer)
      , m_size(other.m_size)
    {
      m_queued_request->add_request_to_signal(this);
    }

    ReadRequest::ReadRequest(ReadRequest&& other)
      : m_filepath(rsl::exchange(other.m_filepath, ""))
      , m_queued_request(rsl::exchange(other.m_queued_request, nullptr))
      , m_is_done(rsl::exchange(other.m_is_done, false))
      , m_buffer(rsl::exchange(other.m_buffer, nullptr))
      , m_size(rsl::exchange(other.m_size, 0_bytes))
    {
      m_queued_request->swap_request_to_signal(&other, this);
    }

    ReadRequest::~ReadRequest()
    {
      if (m_queued_request)
      {
        m_queued_request->remove_request_to_signal(this);
      }
    }

    ReadRequest& ReadRequest::operator=(const ReadRequest& other) // NOLINT(bugprone-unhandled-self-assignment)
    {
      REX_ASSERT_X(this != &other, "assigning a read request to itself");

      m_queued_request->remove_request_to_signal(this);

      m_filepath = other.m_filepath;
      m_queued_request = other.m_queued_request;
      m_is_done = other.m_is_done;
      m_buffer = other.m_buffer;
      m_size = other.m_size;

      m_queued_request->add_request_to_signal(this);

      return *this;
    }
    ReadRequest& ReadRequest::operator=(ReadRequest&& other)
    {
      REX_ASSERT_X(this != &other, "moving a read request to itself");

      m_filepath = rsl::exchange(other.m_filepath, "");
      m_queued_request = rsl::exchange(other.m_queued_request, nullptr);
      m_is_done = rsl::exchange(other.m_is_done, false);
      m_buffer = rsl::exchange(other.m_buffer, nullptr);
      m_size = rsl::exchange(other.m_size, 0_bytes);

      m_queued_request->swap_request_to_signal(&other, this);

      return *this;
    }

    void ReadRequest::signal(const rsl::byte* buffer, rsl::memory_size size)
    {
      m_is_done = true;
      m_buffer = buffer;
      m_size = size;
    }

    void ReadRequest::wait() const
    {
      // this should ideally be solved with fibers..
      while (!m_is_done)
      {
        using namespace rsl::chrono_literals; // NOLINT(google-build-using-namespace)
        rsl::this_thread::sleep_for(1ms);
      }
    }

    const rsl::byte* ReadRequest::data() const
    {
      return m_buffer;
    }
    rsl::memory_size ReadRequest::count() const
    {
      return m_size;
    }

    rsl::string_view ReadRequest::filepath() const
    {
      return m_filepath;
    }
	}
}