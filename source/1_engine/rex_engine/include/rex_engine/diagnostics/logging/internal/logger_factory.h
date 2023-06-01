#include "rex_engine/diagnostics/logging/internal/logger.h"
#include "rex_engine/diagnostics/logging/internal/async_logger.h"

namespace rexlog
{
    namespace internal
    {
        //-------------------------------------------------------------------------
        // sync Logger factory - creates sync loggers.
        struct SyncFactoryImpl
        {
            template <typename Sink, typename... SinkArgs>
            static rsl::shared_ptr<rexlog::Logger> create(rex::DebugString loggerName, SinkArgs&&... args)
            {
                auto sink = rsl::allocate_shared<Sink>(rex::global_debug_allocator(), rsl::forward<SinkArgs>(args)...);
                auto new_logger = rsl::allocate_shared<rexlog::Logger>(rex::global_debug_allocator(), rsl::move(loggerName), rsl::move(sink));
                details::Registry::instance().initialize_logger(new_logger);
                return new_logger;
            }
        };

        //-------------------------------------------------------------------------
        // async Logger factory - creates async loggers backed with thread pool.
        // if a global thread pool doesn't already exist, create it with default queue
        // size of 8192 items and single thread.
        template <AsyncOverflowPolicy OverflowPolicy = AsyncOverflowPolicy::Block>
        struct AsyncFactoryImpl
        {
            template <typename Sink, typename... SinkArgs>
            static rsl::shared_ptr<AsyncLogger> create(rex::DebugString loggerName, SinkArgs&&... args)
            {
                constexpr card32 g_default_async_q_size = 8192;

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
    };

    using sync_factory = internal::SyncFactoryImpl;

    //-------------------------------------------------------------------------
    template <typename Sink, typename... SinkArgs>
    rsl::shared_ptr<rexlog::Logger> create(rex::DebugString loggerName, SinkArgs&&... sinkArgs)
    {
        return sync_factory::create<Sink>(rsl::move(loggerName), rsl::forward<SinkArgs>(sinkArgs)...);
    }

    using async_factory = internal::AsyncFactoryImpl<AsyncOverflowPolicy::Block>;
    using async_factory_nonblock = internal::AsyncFactoryImpl<AsyncOverflowPolicy::OverrunOldest>;

    //-------------------------------------------------------------------------
    template <typename Sink, typename... SinkArgs>
    rsl::shared_ptr<rexlog::Logger> create_async(rex::DebugString loggerName, SinkArgs&&... sinkArgs)
    {
        return async_factory::create<Sink>(rsl::move(loggerName), rsl::forward<SinkArgs>(sinkArgs)...);
    }

    //-------------------------------------------------------------------------
    template <typename Sink, typename... SinkArgs>
    rsl::shared_ptr<rexlog::Logger> create_async_nb(rex::DebugString loggerName, SinkArgs&&... sinkArgs)
    {
        return async_factory_nonblock::create<Sink>(rsl::move(loggerName), rsl::forward<SinkArgs>(sinkArgs)...);
    }
}