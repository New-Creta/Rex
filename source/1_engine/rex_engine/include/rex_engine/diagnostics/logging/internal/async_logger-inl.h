

#pragma once

#include "rex_std/memory.h"

#include <rex_engine/diagnostics/logging/internal/async_logger.h>
#include <rex_engine/diagnostics/logging/internal/details/thread_pool.h>
#include <rex_engine/diagnostics/logging/internal/sinks/sink.h>
#include <string>

REXLOG_INLINE rexlog::async_logger::async_logger(rex::DebugString logger_name, sinks_init_list sinks_list, rsl::weak_ptr<details::thread_pool> tp, async_overflow_policy overflow_policy)
    : async_logger(rsl::move(logger_name), sinks_list.begin(), sinks_list.end(), rsl::move(tp), overflow_policy)
{
}

REXLOG_INLINE rexlog::async_logger::async_logger(rex::DebugString logger_name, sink_ptr single_sink, rsl::weak_ptr<details::thread_pool> tp, async_overflow_policy overflow_policy)
    : async_logger(rsl::move(logger_name), {rsl::move(single_sink)}, rsl::move(tp), overflow_policy)
{
}

// send the log message to the thread pool
REXLOG_INLINE void rexlog::async_logger::sink_it_(const details::log_msg& msg)
{
  if(auto pool_ptr = thread_pool_.lock())
  {
    pool_ptr->post_log(shared_from_this(), msg, overflow_policy_);
  }
  else
  {
    throw_rexlog_ex(rex::DebugString("async log: thread pool doesn't exist anymore"));
  }
}

// send flush request to the thread pool
REXLOG_INLINE void rexlog::async_logger::flush_()
{
  if(auto pool_ptr = thread_pool_.lock())
  {
    pool_ptr->post_flush(shared_from_this(), overflow_policy_);
  }
  else
  {
    throw_rexlog_ex(rex::DebugString("async flush: thread pool doesn't exist anymore"));
  }
}

//
// backend functions - called from the thread pool to do the actual job
//
REXLOG_INLINE void rexlog::async_logger::backend_sink_it_(const details::log_msg& msg)
{
  for(auto& sink: sinks_)
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

  if(should_flush_(msg))
  {
    backend_flush_();
  }
}

REXLOG_INLINE void rexlog::async_logger::backend_flush_()
{
  for(auto& sink: sinks_)
  {
    REXLOG_TRY
    {
      sink->flush();
    }
    REXLOG_LOGGER_CATCH(source_loc())
  }
}

REXLOG_INLINE rsl::shared_ptr<rexlog::logger> rexlog::async_logger::clone(rex::DebugString new_name)
{
  auto cloned   = rsl::allocate_shared<rexlog::async_logger>(rex::global_debug_allocator(), *this);
  cloned->name_ = rsl::move(new_name);
  return cloned;
}
