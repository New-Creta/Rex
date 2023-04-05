

#pragma once

#include "rex_engine/memory/global_allocator.h"
#include "rex_engine/types.h"

#include <cassert>
#include "rex_engine/diagnostics/logging/internal/common.h"
#include "rex_engine/diagnostics/logging/internal/details/thread_pool.h"

namespace rexlog
{
  namespace details
  {

    REXLOG_INLINE thread_pool::thread_pool(size_t q_max_items, size_t threads_n, rsl::function<void()> on_thread_start, rsl::function<void()> on_thread_stop)
        : m_q(q_max_items)
    {
      if(threads_n == 0 || threads_n > 1000)
      {
        throw_rexlog_ex(rex::DebugString("rexlog::thread_pool(): invalid threads_n param (valid range is 1-1000)", rex::global_debug_allocator()));
      }
      for(size_t i = 0; i < threads_n; i++)
      {
        threads_.emplace_back(
            [this, on_thread_start, on_thread_stop]
            {
              on_thread_start();
              this->thread_pool::worker_loop_impl();
              on_thread_stop();
            });
      }
    }

    REXLOG_INLINE thread_pool::thread_pool(size_t q_max_items, size_t threads_n, rsl::function<void()> on_thread_start)
        : thread_pool(q_max_items, threads_n, on_thread_start, [] {})
    {
    }

    REXLOG_INLINE thread_pool::thread_pool(size_t q_max_items, size_t threads_n)
        : thread_pool(
              q_max_items, threads_n, [] {}, [] {})
    {
    }

    // message all threads to terminate gracefully join them
    REXLOG_INLINE thread_pool::~thread_pool()
    {
      REXLOG_TRY
      {
        for(size_t i = 0; i < threads_.size(); i++)
        {
          post_async_msg_impl(async_msg(async_msg_type::terminate), async_overflow_policy::block);
        }

        for(auto& t: threads_)
        {
          t.join();
        }
      }
      REXLOG_CATCH_STD
    }

    void REXLOG_INLINE thread_pool::post_log(async_logger_ptr&& worker_ptr, const details::LogMsg& msg, async_overflow_policy overflow_policy)
    {
      async_msg async_m(rsl::move(worker_ptr), async_msg_type::log, msg);
      post_async_msg_impl(rsl::move(async_m), overflow_policy);
    }

    void REXLOG_INLINE thread_pool::post_flush(async_logger_ptr&& worker_ptr, async_overflow_policy overflow_policy)
    {
      post_async_msg_impl(async_msg(rsl::move(worker_ptr), async_msg_type::flush), overflow_policy);
    }

    size_t REXLOG_INLINE thread_pool::overrun_counter()
    {
      return m_q.overrun_counter();
    }

    void REXLOG_INLINE thread_pool::reset_overrun_counter()
    {
      m_q.reset_overrun_counter();
    }

    size_t REXLOG_INLINE thread_pool::queue_size()
    {
      return m_q.size();
    }

    void REXLOG_INLINE thread_pool::post_async_msg_impl(async_msg&& new_msg, async_overflow_policy overflow_policy)
    {
      if(overflow_policy == async_overflow_policy::block)
      {
        m_q.enqueue(rsl::move(new_msg));
      }
      else
      {
        m_q.enqueue_nowait(rsl::move(new_msg));
      }
    }

    void REXLOG_INLINE thread_pool::worker_loop_impl()
    {
      while(process_next_msg_impl())
      {
      }
    }

    // process next message in the queue
    // return true if this thread should still be active (while no terminate msg
    // was received)
    bool REXLOG_INLINE thread_pool::process_next_msg_impl()
    {
      async_msg incoming_async_msg;
      m_q.dequeue(incoming_async_msg);

      switch(incoming_async_msg.msg_type)
      {
        case async_msg_type::log:
        {
          incoming_async_msg.worker_ptr->backend_sink_it_impl(incoming_async_msg);
          return true;
        }
        case async_msg_type::flush:
        {
          incoming_async_msg.worker_ptr->backend_flush_impl();
          return true;
        }

        case async_msg_type::terminate:
        {
          return false;
        }

        default:
        {
          assert(false);
        }
      }

      return true;
    }

  } // namespace details
} // namespace rexlog
