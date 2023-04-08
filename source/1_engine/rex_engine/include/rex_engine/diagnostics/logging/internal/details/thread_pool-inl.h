

#pragma once

#include "rex_engine/diagnostics/logging/internal/common.h"
#include "rex_engine/diagnostics/logging/internal/details/thread_pool.h"
#include "rex_engine/memory/global_allocator.h"
#include "rex_engine/types.h"

#include <cassert>
#include <utility>

namespace rexlog
{
  namespace details
  {

    REXLOG_INLINE ThreadPool::ThreadPool(size_t qMaxItems, size_t threadsN, const rsl::function<void()>& onThreadStart, const rsl::function<void()>& onThreadStop)
        : m_q(qMaxItems)
    {
      if(threadsN == 0 || threadsN > 1000)
      {
        throw_rexlog_ex(rex::DebugString("rexlog::ThreadPool(): invalid threads_n param (valid range is 1-1000)", rex::global_debug_allocator()));
      }
      for(size_t i = 0; i < threadsN; i++)
      {
        m_threads.emplace_back(
            [this, onThreadStart, onThreadStop]
            {
              onThreadStart();
              this->ThreadPool::worker_loop_impl();
              onThreadStop();
            });
      }
    }

    REXLOG_INLINE ThreadPool::ThreadPool(size_t qMaxItems, size_t threadsN, const rsl::function<void()>& onThreadStart)
        : ThreadPool(qMaxItems, threadsN, onThreadStart, [] {})
    {
    }

    REXLOG_INLINE ThreadPool::ThreadPool(size_t qMaxItems, size_t threadsN)
        : ThreadPool(
              qMaxItems, threadsN, [] {}, [] {})
    {
    }

    // message all threads to terminate gracefully join them
    REXLOG_INLINE ThreadPool::~ThreadPool()
    {
      REXLOG_TRY
      {
        for(size_t i = 0; i < m_threads.size(); i++)
        {
          post_async_msg_impl(AsyncMsg(AsyncMsgType::Terminate), AsyncOverflowPolicy::Block);
        }

        for(auto& t: m_threads)
        {
          t.join();
        }
      }
      REXLOG_CATCH_STD
    }

    void REXLOG_INLINE ThreadPool::post_log(async_logger_ptr&& workerPtr, const details::LogMsg& msg, AsyncOverflowPolicy overflowPolicy)
    {
      AsyncMsg async_m(rsl::move(workerPtr), AsyncMsgType::Log, msg);
      post_async_msg_impl(rsl::move(async_m), overflowPolicy);
    }

    REXLOG_INLINE void ThreadPool::post_flush(async_logger_ptr&& workerPtr, AsyncOverflowPolicy overflowPolicy)
    {
      post_async_msg_impl(AsyncMsg(rsl::move(workerPtr), AsyncMsgType::Flush), overflowPolicy);
    }

    REXLOG_INLINE size_t ThreadPool::overrun_counter()
    {
      return m_q.overrun_counter();
    }

    REXLOG_INLINE void  ThreadPool::reset_overrun_counter()
    {
      m_q.reset_overrun_counter();
    }

    REXLOG_INLINE size_t  ThreadPool::queue_size()
    {
      return m_q.size();
    }

    REXLOG_INLINE void  ThreadPool::post_async_msg_impl(AsyncMsg&& newMsg, AsyncOverflowPolicy overflowPolicy)
    {
      if(overflowPolicy == AsyncOverflowPolicy::Block)
      {
        m_q.enqueue(rsl::move(newMsg));
      }
      else
      {
        m_q.enqueue_nowait(rsl::move(newMsg));
      }
    }

    REXLOG_INLINE void  ThreadPool::worker_loop_impl()
    {
      while(process_next_msg_impl())
      {
      }
    }

    // process next message in the queue
    // return true if this thread should still be active (while no terminate msg
    // was received)
    REXLOG_INLINE bool  ThreadPool::process_next_msg_impl()
    {
      AsyncMsg incoming_async_msg;
      m_q.dequeue(incoming_async_msg);

      switch(incoming_async_msg.msg_type)
      {
        case AsyncMsgType::Log:
        {
          incoming_async_msg.worker_ptr->backend_sink_it_impl(incoming_async_msg);
          return true;
        }
        case AsyncMsgType::Flush:
        {
          incoming_async_msg.worker_ptr->backend_flush_impl();
          return true;
        }

        case AsyncMsgType::Terminate:
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
