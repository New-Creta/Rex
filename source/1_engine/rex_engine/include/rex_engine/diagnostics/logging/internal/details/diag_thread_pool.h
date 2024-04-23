

#pragma once

#include "rex_engine/diagnostics/logging/internal/async_logger.h"
#include "rex_engine/diagnostics/logging/internal/details/log_msg_buffer.h"
#include "rex_engine/diagnostics/logging/internal/details/mpmc_blocking_q.h"
#include "rex_engine/diagnostics/logging/internal/details/os.h"
#include "rex_engine/engine/debug_types.h"
#include "rex_engine/engine/types.h"
#include "rex_std/functional.h"
#include "rex_std/memory.h"
#include "rex_std/thread.h"

namespace rex
{
  namespace log
  {
    namespace details
    {
      class LogMsg;

      using async_logger_ptr = rsl::shared_ptr<rex::log::AsyncLogger>;

      enum class AsyncMsgType
      {
        Log,
        Flush,
        Terminate
      };

      struct AsyncMsgLogFunctions
      {
        using LogFunction   = rsl::function<void(const details::LogMsg&)>;
        using FlushFunction = rsl::function<void()>;

        LogFunction log_fn;
        FlushFunction flush_fn;
      };

      // Async msg to move to/from the queue movable only
      // This should never be copied
      struct AsyncMsg : LogMsgBuffer
      {
        AsyncMsgType msg_type {AsyncMsgType::Log};
        AsyncMsgLogFunctions logger_fns;

        AsyncMsg()           = default;
        ~AsyncMsg() override = default;

        AsyncMsg(const AsyncMsg&)            = delete;
        AsyncMsg(AsyncMsg&&)                 = default;
        AsyncMsg& operator=(const AsyncMsg&) = delete;
        AsyncMsg& operator=(AsyncMsg&&)      = default;

        AsyncMsg(AsyncMsgLogFunctions&& fns, AsyncMsgType theType, const details::LogMsg& m)
            : LogMsgBuffer {m}
            , msg_type {theType}
            , logger_fns {rsl::move(fns)}
        {
        }

        AsyncMsg(AsyncMsgLogFunctions&& fns, AsyncMsgType theType)
            : LogMsgBuffer {}
            , msg_type {theType}
            , logger_fns {rsl::move(fns)}
        {
        }

        explicit AsyncMsg(AsyncMsgType theType)
            : AsyncMsg {{}, theType}
        {
        }
      };

      class ThreadPool
      {
      public:
        using item_type = AsyncMsg;
        using q_type    = details::MpmcBlockingQueue<item_type>;

        ThreadPool(s32 qMaxItems, s32 threadsN, const rsl::function<void()>& onThreadStart, const rsl::function<void()>& onThreadStop);
        ThreadPool(s32 qMaxItems, s32 threadsN, const rsl::function<void()>& onThreadStart);
        ThreadPool(s32 qMaxItems, s32 threadsN);

        // message all threads to terminate gracefully and join them
        ~ThreadPool();

        ThreadPool(const ThreadPool&)            = delete;
        ThreadPool(ThreadPool&&)                 = delete;
        ThreadPool& operator=(const ThreadPool&) = delete;
        ThreadPool& operator=(ThreadPool&&)      = delete;

        void post_log(AsyncMsgLogFunctions&& loggerFns, const details::LogMsg& msg, AsyncOverflowPolicy overflowPolicy);
        void post_flush(AsyncMsgLogFunctions&& loggerFns, AsyncOverflowPolicy overflowPolicy);
        s32 overrun_counter();
        void reset_overrun_counter();
        s32 queue_size();

      private:
        void post_async_msg_impl(AsyncMsg&& newMsg, AsyncOverflowPolicy overflowPolicy);
        void worker_loop_impl();

        // process next message in the queue
        // return true if this thread should still be active
        // while no terminate msg was received
        bool process_next_msg_impl();

      private:
        q_type m_q;

        rex::DebugVector<rsl::thread> m_threads;
      };

    } // namespace details
  }   // namespace log
} // namespace rex