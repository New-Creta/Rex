

#pragma once

#include "rex_engine/diagnostics/logging/internal/async_logger.h"
#include "rex_engine/diagnostics/logging/internal/details/thread_pool.h"
#include "rex_engine/diagnostics/logging/internal/sinks/sink.h"
#include "rex_std/memory.h"

#include <string>

REXLOG_INLINE rexlog::AsyncLogger::AsyncLogger(rex::DebugString loggerName, sinks_init_list sinksList, rsl::weak_ptr<details::ThreadPool> tp, AsyncOverflowPolicy overflowPolicy)
    : AsyncLogger(rsl::move(loggerName), sinksList.begin(), sinksList.end(), rsl::move(tp), overflowPolicy)
{
}

REXLOG_INLINE rexlog::AsyncLogger::AsyncLogger(rex::DebugString loggerName, sink_ptr singleSink, rsl::weak_ptr<details::ThreadPool> tp, AsyncOverflowPolicy overflowPolicy)
    : AsyncLogger(rsl::move(loggerName), {rsl::move(singleSink)}, rsl::move(tp), overflowPolicy)
{
}

// send the log message to the thread pool
REXLOG_INLINE void rexlog::AsyncLogger::sink_it_impl(const details::LogMsg& msg)
{
  if(auto pool_ptr = m_thread_pool.lock())
  {
    pool_ptr->post_log(shared_from_this(), msg, m_overflow_policy);
  }
  else
  {
    throw_rexlog_ex(rex::DebugString("async log: thread pool doesn't exist anymore"));
  }
}

// send flush request to the thread pool
REXLOG_INLINE void rexlog::AsyncLogger::flush_impl()
{
  if(auto pool_ptr = m_thread_pool.lock())
  {
    pool_ptr->post_flush(shared_from_this(), m_overflow_policy);
  }
  else
  {
    throw_rexlog_ex(rex::DebugString("async flush: thread pool doesn't exist anymore"));
  }
}

//
// backend functions - called from the thread pool to do the actual job
//
REXLOG_INLINE void rexlog::AsyncLogger::backend_sink_it_impl(const details::LogMsg& msg)
{
  for(auto& sink: sinks())
  {
    if(sink->should_log(msg.level))
    {
      REXLOG_TRY
      {
        sink->log(msg);
      }
      REXLOG_LOGGER_CATCH(msg.source)
    }
  }

  if(should_flush_impl(msg))
  {
    backend_flush_impl();
  }
}

REXLOG_INLINE void rexlog::AsyncLogger::backend_flush_impl()
{
  for(auto& sink: sinks())
  {
    REXLOG_TRY
    {
      sink->flush();
    }
    REXLOG_LOGGER_CATCH(source_loc())
  }
}

REXLOG_INLINE rsl::shared_ptr<rexlog::Logger> rexlog::AsyncLogger::clone(rex::DebugString newName)
{
  auto cloned = rsl::allocate_shared<rexlog::AsyncLogger>(rex::global_debug_allocator(), *this);
  cloned->set_name(rsl::move(newName));
  return cloned;
}
