#include "rex_engine/diagnostics/logging/internal/async_logger.h"
#include "rex_engine/diagnostics/logging/internal/logger.h"

namespace rex
{
  namespace log
  {
    namespace internal
    {
      //-------------------------------------------------------------------------
      // sync Logger factory - creates sync loggers.
      struct SyncFactoryImpl
      {
        template <typename Sink, typename... SinkArgs>
        static rsl::shared_ptr<rex::log::Logger> create(rsl::string_view loggerName, SinkArgs&&... args)
        {
          auto sink       = rsl::allocate_shared<Sink>(rex::GlobalDebugAllocator(), rsl::forward<SinkArgs>(args)...);
          auto new_logger = rsl::allocate_shared<rex::log::Logger>(rex::GlobalDebugAllocator(), rsl::move(loggerName), rsl::move(sink));
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
        static rsl::shared_ptr<AsyncLogger> create(rsl::string_view loggerName, SinkArgs&&... args)
        {
          constexpr s32 g_default_async_q_size = 8192;

          auto& registry_inst = details::Registry::instance();

          // create global thread pool if not already exists..
          const rsl::unique_lock<rsl::recursive_mutex> tp_lock(registry_inst.thread_pool_mutex());

          auto tp = registry_inst.thread_pool();
          if(tp == nullptr)
          {
            tp = rsl::allocate_shared<details::ThreadPool>(rex::GlobalDebugAllocator(), g_default_async_q_size, 1U);
            registry_inst.set_thread_pool(tp);
          }

          auto sink       = rsl::allocate_shared<Sink>(rex::GlobalDebugAllocator(), rsl::forward<SinkArgs>(args)...);
          auto new_logger = rsl::allocate_shared<AsyncLogger>(rex::GlobalDebugAllocator(), rsl::move(loggerName), rsl::move(sink), rsl::move(tp), OverflowPolicy);
          registry_inst.initialize_logger(new_logger);
          return new_logger;
        }
      };
    } // namespace internal

    using sync_factory = internal::SyncFactoryImpl;

    //-------------------------------------------------------------------------
    template <typename Sink, typename... SinkArgs>
    rsl::shared_ptr<rex::log::Logger> create(rsl::string_view loggerName, SinkArgs&&... sinkArgs)
    {
      return sync_factory::create<Sink>(loggerName, rsl::forward<SinkArgs>(sinkArgs)...);
    }

    using async_factory          = internal::AsyncFactoryImpl<AsyncOverflowPolicy::Block>;
    using async_factory_nonblock = internal::AsyncFactoryImpl<AsyncOverflowPolicy::OverrunOldest>;

    //-------------------------------------------------------------------------
    template <typename Sink, typename... SinkArgs>
    rsl::shared_ptr<rex::log::Logger> create_async(rsl::string_view loggerName, SinkArgs&&... sinkArgs)
    {
      return async_factory::create<Sink>(loggerName, rsl::forward<SinkArgs>(sinkArgs)...);
    }

    //-------------------------------------------------------------------------
    template <typename Sink, typename... SinkArgs>
    rsl::shared_ptr<rex::log::Logger> create_async_nb(rsl::string_view loggerName, SinkArgs&&... sinkArgs)
    {
      return async_factory_nonblock::create<Sink>(loggerName, rsl::forward<SinkArgs>(sinkArgs)...);
    }
  } // namespace log
} // namespace rex