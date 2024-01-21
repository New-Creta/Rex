#include "rex_engine/diagnostics/logging/internal/details/thread_pool.h"

#include "rex_engine/diagnostics/logging/internal/common.h"
#include "rex_engine/engine/types.h"
#include "rex_std/bonus/types.h"

#include <cassert>
#include <stdio.h>

// IWYU pragma: no_include <built-in>

// NOLINTBEGIN(misc-definitions-in-headers)

namespace rex
{
  namespace log
  {
    namespace details
    {

      ThreadPool::ThreadPool(s32 qMaxItems, s32 threadsN, const rsl::function<void()>& onThreadStart, const rsl::function<void()>& onThreadStop)
          : m_q(qMaxItems)
      {
        if(threadsN == 0 || threadsN > 1000)
        {
          printf("rex::log::ThreadPool(): invalid threads_n param (valid range is 1-1000)");
        }
        for(s32 i = 0; i < threadsN; i++)
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

      ThreadPool::ThreadPool(s32 qMaxItems, s32 threadsN, const rsl::function<void()>& onThreadStart)
          : ThreadPool(qMaxItems, threadsN, onThreadStart, [] {})
      {
      }

      ThreadPool::ThreadPool(s32 qMaxItems, s32 threadsN)
          : ThreadPool(
                qMaxItems, threadsN, [] {}, [] {})
      {
      }

      // message all threads to terminate gracefully join them
      ThreadPool::~ThreadPool()
      {
        for(count_t i = 0; i < m_threads.size(); i++)
        {
          post_async_msg_impl(AsyncMsg(AsyncMsgType::Terminate), AsyncOverflowPolicy::Block);
        }

        for(auto& t: m_threads)
        {
          t.join();
        }
      }

      void ThreadPool::post_log(AsyncMsgLogFunctions&& loggerFns, const details::LogMsg& msg, AsyncOverflowPolicy overflowPolicy)
      {
        AsyncMsg async_m(rsl::move(loggerFns), AsyncMsgType::Log, msg);
        post_async_msg_impl(rsl::move(async_m), overflowPolicy);
      }

      void ThreadPool::post_flush(AsyncMsgLogFunctions&& loggerFns, AsyncOverflowPolicy overflowPolicy)
      {
        post_async_msg_impl(AsyncMsg(rsl::move(loggerFns), AsyncMsgType::Flush), overflowPolicy);
      }

      s32 ThreadPool::overrun_counter()
      {
        return m_q.overrun_counter();
      }

      void ThreadPool::reset_overrun_counter()
      {
        m_q.reset_overrun_counter();
      }

      s32 ThreadPool::queue_size()
      {
        return m_q.size();
      }

      void ThreadPool::post_async_msg_impl(AsyncMsg&& newMsg, AsyncOverflowPolicy overflowPolicy)
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

      void ThreadPool::worker_loop_impl()
      {
        while(process_next_msg_impl())
        {
        }
      }

      // process next message in the queue
      // return true if this thread should still be active (while no terminate msg
      // was received)
      bool ThreadPool::process_next_msg_impl()
      {
        AsyncMsg incoming_async_msg;
        m_q.dequeue(incoming_async_msg);

        switch(incoming_async_msg.msg_type)
        {
          case AsyncMsgType::Log:
          {
            incoming_async_msg.logger_fns.log_fn(incoming_async_msg);
            return true;
          }
          case AsyncMsgType::Flush:
          {
            incoming_async_msg.logger_fns.flush_fn();
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
  }   // namespace log
} // namespace rex

// NOLINTEND(misc-definitions-in-headers)
