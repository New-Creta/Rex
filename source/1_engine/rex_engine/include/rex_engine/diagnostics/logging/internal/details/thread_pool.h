

#pragma once

#include "rex_engine/types.h"
#include "rex_std/chrono.h"
#include "rex_std/memory.h"
#include "rex_std/thread.h"
#include "rex_std/vector.h"

#include "rex_engine/diagnostics/logging/internal/details/log_msg_buffer.h"
#include "rex_engine/diagnostics/logging/internal/details/mpmc_blocking_q.h"
#include "rex_engine/diagnostics/logging/internal/details/os.h"

namespace rexlog
{
  class AsyncLogger;

  namespace details
  {

    using async_logger_ptr = rsl::shared_ptr<rexlog::AsyncLogger>;

    enum class AsyncMsgType
    {
      Log,
      Flush,
      Terminate
    };

    // Async msg to move to/from the queue
    // Movable only. should never be copied
    struct AsyncMsg : LogMsgBuffer
    {
      AsyncMsgType msg_type {AsyncMsgType::Log};
      async_logger_ptr worker_ptr;

      AsyncMsg()  = default;
      ~AsyncMsg() = default;

      // should only be moved in or out of the queue..
      AsyncMsg(const AsyncMsg&) = delete;

// support for vs2013 move
#if defined(_MSC_VER) && _MSC_VER <= 1800
      async_msg(async_msg&& other)
          : LogMsgBuffer(rsl::move(other))
          , msg_type(other.msg_type)
          , worker_ptr(rsl::move(other.worker_ptr))
      {
      }

      async_msg& operator=(async_msg&& other)
      {
        *static_cast<LogMsgBuffer*>(this) = rsl::move(other);
        msg_type                            = other.msg_type;
        worker_ptr                          = rsl::move(other.worker_ptr);
        return *this;
      }
#else // (_MSC_VER) && _MSC_VER <= 1800
      AsyncMsg(AsyncMsg&&)            = default;
      AsyncMsg& operator=(const AsyncMsg&) = delete;
      AsyncMsg& operator=(AsyncMsg&&) = default;
#endif

      // construct from LogMsg with given type
      AsyncMsg(async_logger_ptr&& worker, AsyncMsgType theType, const details::LogMsg& m)
          : LogMsgBuffer {m}
          , msg_type {theType}
          , worker_ptr {rsl::move(worker)}
      {
      }

      AsyncMsg(async_logger_ptr&& worker, AsyncMsgType theType)
          : LogMsgBuffer {}
          , msg_type {theType}
          , worker_ptr {rsl::move(worker)}
      {
      }

      explicit AsyncMsg(AsyncMsgType theType)
          : AsyncMsg {nullptr, theType}
      {
      }
    };

    class REXLOG_API ThreadPool
    {
    public:
      using item_type = AsyncMsg;
      using q_type    = details::MpmcBlockingQueue<item_type>;

      ThreadPool(size_t qMaxItems, size_t threadsN, const rsl::function<void()>& onThreadStart, const rsl::function<void()>& onThreadStop);
      ThreadPool(size_t qMaxItems, size_t threadsN, const rsl::function<void()>& onThreadStart);
      ThreadPool(size_t qMaxItems, size_t threadsN);

      // message all threads to terminate gracefully and join them
      ~ThreadPool();

      ThreadPool(const ThreadPool&)       = delete;
      ThreadPool& operator=(ThreadPool&&) = delete;

      ThreadPool& operator=(const ThreadPool&) = delete;
      ThreadPool& operator=(ThreadPool&&) = delete;

      void post_log(async_logger_ptr&& workerPtr, const details::LogMsg& msg, AsyncOverflowPolicy overflowPolicy);
      void post_flush(async_logger_ptr&& workerPtr, AsyncOverflowPolicy overflowPolicy);
      size_t overrun_counter();
      void reset_overrun_counter();
      size_t queue_size();

    private:
      q_type m_q;

      rex::DebugVector<rsl::thread> m_threads;

      void post_async_msg_impl(AsyncMsg&& newMsg, AsyncOverflowPolicy overflowPolicy);
      void worker_loop_impl();

      // process next message in the queue
      // return true if this thread should still be active (while no terminate msg
      // was received)
      bool process_next_msg_impl();
    };

  } // namespace details
} // namespace rexlog