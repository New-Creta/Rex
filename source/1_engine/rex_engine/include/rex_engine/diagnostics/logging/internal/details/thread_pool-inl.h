

#pragma once

#include <cassert>
#include <rex_engine/diagnostics/logging/internal/common.h>
#include <rex_engine/diagnostics/logging/internal/details/thread_pool.h>

namespace rexlog
{
  namespace details
  {

    REXLOG_INLINE thread_pool::thread_pool(size_t q_max_items, size_t threads_n, rsl::function<void()> on_thread_start, rsl::function<void()> on_thread_stop)
        : q_(q_max_items)
    {
      if(threads_n == 0 || threads_n > 1000)
      {
        throw_rexlog_ex(rsl::string("rexlog::thread_pool(): invalid threads_n param (valid range is 1-1000)"));
      }
      for(size_t i = 0; i < threads_n; i++)
      {
        threads_.emplace_back(
            [this, on_thread_start, on_thread_stop]
            {
              on_thread_start();
              this->thread_pool::worker_loop_();
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
          post_async_msg_(async_msg(async_msg_type::terminate), async_overflow_policy::block);
        }

        for(auto& t: threads_)
        {
          t.join();
        }
      }
      REXLOG_CATCH_STD
    }

    void REXLOG_INLINE thread_pool::post_log(async_logger_ptr&& worker_ptr, const details::log_msg& msg, async_overflow_policy overflow_policy)
    {
      async_msg async_m(rsl::move(worker_ptr), async_msg_type::log, msg);
      post_async_msg_(rsl::move(async_m), overflow_policy);
    }

    void REXLOG_INLINE thread_pool::post_flush(async_logger_ptr&& worker_ptr, async_overflow_policy overflow_policy)
    {
      post_async_msg_(async_msg(rsl::move(worker_ptr), async_msg_type::flush), overflow_policy);
    }

    size_t REXLOG_INLINE thread_pool::overrun_counter()
    {
      return q_.overrun_counter();
    }

    void REXLOG_INLINE thread_pool::reset_overrun_counter()
    {
      q_.reset_overrun_counter();
    }

    size_t REXLOG_INLINE thread_pool::queue_size()
    {
      return q_.size();
    }

    void REXLOG_INLINE thread_pool::post_async_msg_(async_msg&& new_msg, async_overflow_policy overflow_policy)
    {
      if(overflow_policy == async_overflow_policy::block)
      {
        q_.enqueue(rsl::move(new_msg));
      }
      else
      {
        q_.enqueue_nowait(rsl::move(new_msg));
      }
    }

    void REXLOG_INLINE thread_pool::worker_loop_()
    {
      while(process_next_msg_())
      {
      }
    }

    // process next message in the queue
    // return true if this thread should still be active (while no terminate msg
    // was received)
    bool REXLOG_INLINE thread_pool::process_next_msg_()
    {
      async_msg incoming_async_msg;
      q_.dequeue(incoming_async_msg);

      switch(incoming_async_msg.msg_type)
      {
        case async_msg_type::log:
        {
          incoming_async_msg.worker_ptr->backend_sink_it_(incoming_async_msg);
          return true;
        }
        case async_msg_type::flush:
        {
          incoming_async_msg.worker_ptr->backend_flush_();
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