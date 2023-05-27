

#pragma once

// Fast asynchronous Logger.
// Uses pre allocated queue.
// Creates a single back thread to pop messages from the queue and log them.
//
// Upon each log write the Logger:
//    1. Checks if its log level is enough to log the message
//    2. Push a new copy of the message to a queue (or block the caller until
//    space is available in the queue)
// Upon destruction, logs all remaining messages in the queue before
// destructing..

#include "rex_engine/diagnostics/logging/internal/logger.h"
#include "rex_std/memory.h"

namespace rexlog
{
    // Async overflow policy - block by default.
    enum class AsyncOverflowPolicy
    {
        Block,        // Block until message can be enqueued
        OverrunOldest // Discard oldest message in the queue if full when trying to add new item.
    };

    namespace details
    {
        class ThreadPool;
    } // namespace details

    class AsyncLogger final : public rsl::enable_shared_from_this<AsyncLogger>, public Logger // NOLINT(fuchsia-multiple-inheritance)
    {
        friend class details::ThreadPool;

    public:
        template <typename It>
        AsyncLogger(rex::DebugString loggerName, It begin, It end, rsl::weak_ptr<details::ThreadPool> tp, AsyncOverflowPolicy overflowPolicy = AsyncOverflowPolicy::Block)
            : Logger(rsl::move(loggerName), begin, end)
            , m_thread_pool(rsl::move(tp))
            , m_overflow_policy(overflowPolicy)
        {
            // Nothing to implement
        }

        AsyncLogger(rex::DebugString loggerName, sinks_init_list sinksList, rsl::weak_ptr<details::ThreadPool> tp, AsyncOverflowPolicy overflowPolicy = AsyncOverflowPolicy::Block);
        AsyncLogger(rex::DebugString loggerName, sink_ptr singleSink, rsl::weak_ptr<details::ThreadPool> tp, AsyncOverflowPolicy overflowPolicy = AsyncOverflowPolicy::Block);

        rsl::shared_ptr<Logger> clone(rex::DebugString newName) override;

    protected:
        void sink_it_impl(const details::LogMsg& msg) override;
        void flush_it_impl() override;
        void backend_sink_it_impl(const details::LogMsg& incomingLogMsg);
        void backend_flush_impl();

    private:
        rsl::weak_ptr<details::ThreadPool> m_thread_pool;
        AsyncOverflowPolicy m_overflow_policy;
    };

    static const size_t g_default_async_q_size = 8192;

    namespace internal
    {
        // async Logger factory - creates async loggers backed with thread pool.
        // if a global thread pool doesn't already exist, create it with default queue
        // size of 8192 items and single thread.
        template <AsyncOverflowPolicy OverflowPolicy = AsyncOverflowPolicy::Block>
        struct AsyncFactoryImpl
        {
            template <typename Sink, typename... SinkArgs>
            static rsl::shared_ptr<AsyncLogger> create(rex::DebugString loggerName, SinkArgs&&... args)
            {
                auto& registry_inst = details::Registry::instance();

                // create global thread pool if not already exists..
                const rsl::unique_lock<rsl::recursive_mutex> tp_lock(registry_inst.tp_mutex());

                auto tp = registry_inst.tp();
                if (tp == nullptr)
                {
                    tp = rsl::allocate_shared<details::ThreadPool>(rex::global_debug_allocator(), g_default_async_q_size, 1U);
                    registry_inst.set_tp(tp);
                }

                auto sink = rsl::allocate_shared<Sink>(rex::global_debug_allocator(), rsl::forward<SinkArgs>(args)...);
                auto new_logger = rsl::allocate_shared<AsyncLogger>(rex::global_debug_allocator(), rsl::move(loggerName), rsl::move(sink), rsl::move(tp), OverflowPolicy);
                registry_inst.initialize_logger(new_logger);
                return rsl::shared_ptr<AsyncLogger>(rsl::move(new_logger));
            }
        };
    }

    using async_factory = internal::AsyncFactoryImpl<AsyncOverflowPolicy::Block>;
    using async_factory_nonblock = internal::AsyncFactoryImpl<AsyncOverflowPolicy::OverrunOldest>;

    template <typename Sink, typename... SinkArgs>
    rsl::shared_ptr<rexlog::Logger> create_async(rex::DebugString loggerName, SinkArgs&&... sinkArgs)
    {
        return async_factory::create<Sink>(rsl::move(loggerName), rsl::forward<SinkArgs>(sinkArgs)...);
    }

    template <typename Sink, typename... SinkArgs>
    rsl::shared_ptr<rexlog::Logger> create_async_nb(rex::DebugString loggerName, SinkArgs&&... sinkArgs)
    {
        return async_factory_nonblock::create<Sink>(rsl::move(loggerName), rsl::forward<SinkArgs>(sinkArgs)...);
    }
} // namespace rexlog