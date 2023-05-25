

#pragma once

//
// Async logging using global thread pool
// All loggers created here share same global thread pool.
// Each log message is pushed to a queue along with a shared pointer to the
// Logger.
// If a Logger deleted while having pending messages in the queue, it's actual
// destruction will defer
// until all its messages are processed by the thread pool.
// This is because each message in the queue holds a shared_ptr to the
// originating Logger.

#include "rex_engine/debug_types.h"
#include "rex_engine/diagnostics/logging/internal/async_logger.h"
#include "rex_engine/diagnostics/logging/internal/details/registry.h"
#include "rex_engine/diagnostics/logging/internal/details/thread_pool.h"
#include "rex_engine/memory/global_allocator.h"
#include "rex_std/functional.h"
#include "rex_std/memory.h"
#include "rex_std/mutex.h"

#include <cstddef>

namespace rexlog
{
    class Logger;

    namespace details
    {
        static const size_t g_default_async_q_size = 8192;
    } // namespace details

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
                tp = rsl::allocate_shared<details::ThreadPool>(rex::global_debug_allocator(), details::g_default_async_q_size, 1U);
                registry_inst.set_tp(tp);
            }

            auto sink = rsl::allocate_shared<Sink>(rex::global_debug_allocator(), rsl::forward<SinkArgs>(args)...);
            auto new_logger = rsl::allocate_shared<AsyncLogger>(rex::global_debug_allocator(), rsl::move(loggerName), rsl::move(sink), rsl::move(tp), OverflowPolicy);
            registry_inst.initialize_logger(new_logger);
            return rsl::shared_ptr<AsyncLogger>(rsl::move(new_logger));
        }
    };
} // namespace rexlog
