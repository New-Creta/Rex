#include "rex_engine/diagnostics/logging/internal/async_logger.h"

#include "rex_engine/diagnostics/logging/internal/details/thread_pool.h"
#include "rex_engine/diagnostics/logging/internal/sinks/sink.h"
#include "rex_engine/memory/global_allocator.h"
#include "rex_std/memory.h"

// NOLINTBEGIN(misc-definitions-in-headers)

namespace rexlog
{
  //-------------------------------------------------------------------------
  rexlog::AsyncLogger::AsyncLogger(rsl::string_view loggerName, sinks_init_list sinksList, rsl::weak_ptr<details::ThreadPool> tp, AsyncOverflowPolicy overflowPolicy)
      : AsyncLogger(loggerName, sinksList.begin(), sinksList.end(), rsl::move(tp), overflowPolicy)
  {
  }

  //-------------------------------------------------------------------------
  rexlog::AsyncLogger::AsyncLogger(rsl::string_view loggerName, sink_ptr singleSink, rsl::weak_ptr<details::ThreadPool> tp, AsyncOverflowPolicy overflowPolicy)
      : AsyncLogger(loggerName, {rsl::move(singleSink)}, rsl::move(tp), overflowPolicy)
  {
  }

  //-------------------------------------------------------------------------
  // send the log message to the thread pool
  void rexlog::AsyncLogger::sink_it_impl(const details::LogMsg& msg)
  {
    if(auto pool_ptr = m_thread_pool.lock())
    {
      pool_ptr->post_log(make_msg_log_functions(), msg, m_overflow_policy);
    }
    else
    {
      printf("async log: thread pool doesn't exist anymore");
    }
  }

  //-------------------------------------------------------------------------
  // send flush request to the thread pool
  void rexlog::AsyncLogger::flush_it_impl()
  {
    if(auto pool_ptr = m_thread_pool.lock())
    {
      pool_ptr->post_flush(make_msg_log_functions(), m_overflow_policy);
    }
    else
    {
      printf("async flush: thread pool doesn't exist anymore");
    }
  }

  //-------------------------------------------------------------------------
  // backend functions - called from the thread pool to do the actual job
  void rexlog::AsyncLogger::backend_sink_it_impl(const details::LogMsg& msg)
  {
    for(auto& sink: sinks())
    {
      if(sink->should_log(msg.level()))
      {
        sink->log(msg);
      }
    }

    if(should_flush_impl(msg))
    {
      backend_flush_impl();
    }
  }

  //-------------------------------------------------------------------------
  void rexlog::AsyncLogger::backend_flush_impl()
  {
    for(auto& sink: sinks())
    {
      sink->flush();
    }
  }

  //-------------------------------------------------------------------------
  rsl::shared_ptr<rexlog::Logger> rexlog::AsyncLogger::clone(rsl::string_view newName)
  {
    auto cloned = rsl::allocate_shared<rexlog::AsyncLogger>(rex::global_debug_allocator(), *this);

    cloned->set_name(newName);

    return cloned;
  }

  //-------------------------------------------------------------------------
  rexlog::details::AsyncMsgLogFunctions AsyncLogger::make_msg_log_functions()
  {
    return {[&](const details::LogMsg& msg) { backend_sink_it_impl(msg); }, [&]() { backend_flush_impl(); }};
  }

} // namespace rexlog

// NOLINTEND(misc-definitions-in-headers)