#pragma once

#include "rex_std/string_view.h"
#include "rex_std/memory.h"
#include "rex_std/bonus/memory.h"

namespace rex
{
	class QueuedRequest;

	// a request to be held by the user who want signal the filesystem to read a file
	// this allows the user to make a file IO request to be completed at a later point in time
	class ReadRequest
	{
	public:
		ReadRequest(rsl::string_view filepath, QueuedRequest* queuedRequest);
		ReadRequest(const ReadRequest& other);
		ReadRequest(ReadRequest&& other);

		~ReadRequest();

		ReadRequest& operator=(const ReadRequest& other);
		ReadRequest& operator=(ReadRequest&& other);

		// Signal this request that the data is read and available for use
		void signal(const rsl::byte* buffer, rsl::memory_size size);

		// Blocking call. Hold the current thread until the read request has finished
		// and the data is available for use
		void wait() const;

		// get the data and its size
		const rsl::byte* data() const;
		rsl::memory_size count() const;

		// return the filepath that the read request was for
		rsl::string_view filepath() const;

	private:
		rsl::string_view m_filepath;
		QueuedRequest* m_queued_request;
		bool m_is_done;
		const rsl::byte* m_buffer;
		rsl::memory_size m_size;
	};
}
