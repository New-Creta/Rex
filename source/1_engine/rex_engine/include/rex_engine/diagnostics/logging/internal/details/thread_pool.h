

#pragma once

#include "rex_engine/diagnostics/logging/internal/details/log_msg_buffer.h"
#include "rex_engine/diagnostics/logging/internal/details/mpmc_blocking_q.h"
#include "rex_engine/diagnostics/logging/internal/details/os.h"
#include "rex_engine/diagnostics/logging/internal/async_logger.h"
#include "rex_engine/types.h"
#include "rex_std/chrono.h"
#include "rex_std/memory.h"
#include "rex_std/thread.h"
#include "rex_std/vector.h"

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

        struct AsyncMsgLogFunctions
        {
            using LogFunction = std::function<void(const details::LogMsg&)>;
            using FlushFunction = std::function<void()>;

            LogFunction log_fn;
            FlushFunction flush_fn;
        };

        // Async msg to move to/from the queue movable only
        // This should never be copied
        struct AsyncMsg : LogMsgBuffer
        {
            AsyncMsgType msg_type{ AsyncMsgType::Log };
            AsyncMsgLogFunctions logger_fns;

            AsyncMsg() = default;
            ~AsyncMsg() = default;

            AsyncMsg(const AsyncMsg&) = delete;
            AsyncMsg(AsyncMsg&&) = default;
            AsyncMsg& operator=(const AsyncMsg&) = delete;
            AsyncMsg& operator=(AsyncMsg&&) = default;

            AsyncMsg(AsyncMsgLogFunctions&& fns, AsyncMsgType theType, const details::LogMsg& m)
                : LogMsgBuffer{ m }
                , msg_type{ theType }
                , logger_fns{ rsl::move(fns) }
            {
            }

            AsyncMsg(AsyncMsgLogFunctions&& fns, AsyncMsgType theType)
                : LogMsgBuffer{}
                , msg_type{ theType }
                , logger_fns{ rsl::move(fns) }
            {
            }

            explicit AsyncMsg(AsyncMsgType theType)
                : AsyncMsg{ {}, theType }
            {
            }
        };

        class ThreadPool
        {
        public:
            using item_type = AsyncMsg;
            using q_type = details::MpmcBlockingQueue<item_type>;

            ThreadPool(card32 qMaxItems, card32 threadsN, const rsl::function<void()>& onThreadStart, const rsl::function<void()>& onThreadStop);
            ThreadPool(card32 qMaxItems, card32 threadsN, const rsl::function<void()>& onThreadStart);
            ThreadPool(card32 qMaxItems, card32 threadsN);

            // message all threads to terminate gracefully and join them
            ~ThreadPool();

            ThreadPool(const ThreadPool&) = delete;
            ThreadPool(ThreadPool&&) = delete;
            ThreadPool& operator=(const ThreadPool&) = delete;
            ThreadPool& operator=(ThreadPool&&) = delete;

            void post_log(AsyncMsgLogFunctions&& loggerFns, const details::LogMsg& msg, AsyncOverflowPolicy overflowPolicy);
            void post_flush(AsyncMsgLogFunctions&& loggerFns, AsyncOverflowPolicy overflowPolicy);
            size_t overrun_counter();
            void reset_overrun_counter();
            size_t queue_size();

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
} // namespace rexlog