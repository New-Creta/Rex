

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

#include "rex_engine/diagnostics/logging/internal/rexlog_logger.h"
#include "rex_std/memory.h"

namespace rexlog
{

  // Async overflow policy - block by default.
  enum class AsyncOverflowPolicy
  {
    Block,        // Block until message can be enqueued
    OverrunOldest // Discard oldest message in the queue if full when trying to
                  // add new item.
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
    }

    AsyncLogger(rex::DebugString loggerName, sinks_init_list sinksList, rsl::weak_ptr<details::ThreadPool> tp, AsyncOverflowPolicy overflowPolicy = AsyncOverflowPolicy::Block);

    AsyncLogger(rex::DebugString loggerName, sink_ptr singleSink, rsl::weak_ptr<details::ThreadPool> tp, AsyncOverflowPolicy overflowPolicy = AsyncOverflowPolicy::Block);

    rsl::shared_ptr<Logger> clone(rex::DebugString newName) override;

  protected:
    void sink_it_impl(const details::LogMsg& msg) override;
    void flush_impl() override;
    void backend_sink_it_impl(const details::LogMsg& incomingLogMsg);
    void backend_flush_impl();

  private:
    rsl::weak_ptr<details::ThreadPool> m_thread_pool;
    AsyncOverflowPolicy m_overflow_policy;
  };
} // namespace rexlog