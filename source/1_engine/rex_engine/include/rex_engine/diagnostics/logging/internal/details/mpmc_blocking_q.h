

#pragma once

// multi producer-multi consumer blocking queue.
// enqueue(..) - will block until room found to put the new message.
// enqueue_nowait(..) - will return immediately with false if no room left in
// the queue.
// dequeue_for(..) - will block until the queue is not empty or timeout have
// passed.

#include "rex_std/condition_variable.h"
#include "rex_std/mutex.h"

#include "rex_engine/diagnostics/logging/internal/details/circular_q.h"

namespace rexlog
{
  namespace details
  {

    template <typename T>
    class MpmcBlockingQueue
    {
    public:
      using item_type = T;
      explicit MpmcBlockingQueue(size_t maxItems)
          : m_q(maxItems)
      {
      }

#ifndef __MINGW32__
      // try to enqueue and block if no room left
      void enqueue(T&& item)
      {
        {
          rsl::unique_lock<rsl::mutex> lock(m_queue_mutex);
          m_pop_cv.wait(lock, [this] { return !this->m_q.full(); });
          m_q.push_back(rsl::move(item));
        }
        m_push_cv.notify_one();
      }

      // enqueue immediately. overrun oldest message in the queue if no room left.
      void enqueue_nowait(T&& item)
      {
        {
          rsl::unique_lock<rsl::mutex> const lock(m_queue_mutex);
          m_q.push_back(rsl::move(item));
        }
        m_push_cv.notify_one();
      }

      // dequeue with a timeout.
      // Return true, if succeeded dequeue item, false otherwise
      bool dequeue_for(T& poppedItem, rsl::chrono::milliseconds waitDuration)
      {
        {
          rsl::unique_lock<rsl::mutex> lock(m_queue_mutex);
          if(!m_push_cv.wait_for(lock, wait_duration, [this] { return !this->m_q.empty(); }))
          {
            return false;
          }
          popped_item = rsl::move(m_q.front());
          m_q.pop_front();
        }
        m_pop_cv.notify_one();
        return true;
      }

      // blocking dequeue without a timeout.
      void dequeue(T& poppedItem)
      {
        {
          rsl::unique_lock<rsl::mutex> lock(m_queue_mutex);
          m_push_cv.wait(lock, [this] { return !this->m_q.empty(); });
          poppedItem = rsl::move(m_q.front());
          m_q.pop_front();
        }
        m_pop_cv.notify_one();
      }

#else
      // apparently mingw deadlocks if the mutex is released before cv.notify_one(),
      // so release the mutex at the very end each function.

      // try to enqueue and block if no room left
      void enqueue(T&& item)
      {
        rsl::unique_lock<rsl::mutex> lock(m_queue_mutex);
        m_pop_cv.wait(lock, [this] { return !this->m_q.full(); });
        m_q.push_back(rsl::move(item));
        m_push_cv.notify_one();
      }

      // enqueue immediately. overrun oldest message in the queue if no room left.
      void enqueue_nowait(T&& item)
      {
        rsl::unique_lock<rsl::mutex> lock(m_queue_mutex);
        m_q.push_back(rsl::move(item));
        m_push_cv.notify_one();
      }

      // dequeue with a timeout.
      // Return true, if succeeded dequeue item, false otherwise
      bool dequeue_for(T& popped_item, rsl::chrono::milliseconds wait_duration)
      {
        rsl::unique_lock<rsl::mutex> lock(m_queue_mutex);
        if(!m_push_cv.wait_for(lock, wait_duration, [this] { return !this->m_q.empty(); }))
        {
          return false;
        }
        popped_item = rsl::move(m_q.front());
        m_q.pop_front();
        m_pop_cv.notify_one();
        return true;
      }

      // blocking dequeue without a timeout.
      void dequeue(T& popped_item)
      {
        rsl::unique_lock<rsl::mutex> lock(m_queue_mutex);
        m_push_cv.wait(lock, [this] { return !this->m_q.empty(); });
        popped_item = rsl::move(m_q.front());
        m_q.pop_front();
        m_pop_cv.notify_one();
      }

#endif

      size_t overrun_counter()
      {
        rsl::unique_lock<rsl::mutex> const lock(m_queue_mutex);
        return m_q.overrun_counter();
      }

      size_t size()
      {
        rsl::unique_lock<rsl::mutex> const lock(m_queue_mutex);
        return m_q.size();
      }

      void reset_overrun_counter()
      {
        rsl::unique_lock<rsl::mutex> const lock(m_queue_mutex);
        m_q.reset_overrun_counter();
      }

    private:
      rsl::mutex m_queue_mutex;
      rsl::condition_variable m_push_cv;
      rsl::condition_variable m_pop_cv;
      rexlog::details::CircularQ<T> m_q;
    };
  } // namespace details
} // namespace rexlog
