

#pragma once

// multi producer-multi consumer blocking queue.
// enqueue(..) - will block until room found to put the new message.
// enqueue_nowait(..) - will return immediately with false if no room left in
// the queue.
// dequeue_for(..) - will block until the queue is not empty or timeout have
// passed.

#include "rex_std/bonus/circular_q.h"
// #include "rex_engine/diagnostics/logging/internal/details/circular_q.h"
#include "rex_std/condition_variable.h"
#include "rex_std/mutex.h"

namespace rex
{
  namespace log
  {
    namespace details
    {
      template <typename T>
      class MpmcBlockingQueue
      {
      public:
        using item_type = T;

        explicit MpmcBlockingQueue(s32 maxItems)
            : m_q(maxItems)
        {
        }

        void enqueue(T&& item)
        {
          {
            rsl::unique_lock<rsl::mutex> lock(m_queue_mutex);
            m_pop_cv.wait(lock, [this] { return !this->m_q.full(); });
            m_q.push_back(rsl::move(item));
          }

          m_push_cv.notify_one();
        }

        void enqueue_nowait(T&& item)
        {
          {
            const rsl::unique_lock<rsl::mutex> lock(m_queue_mutex);
            m_q.push_back(rsl::move(item));
          }

          m_push_cv.notify_one();
        }

        bool dequeue_for(T& poppedItem, rsl::chrono::milliseconds waitDuration)
        {
          {
            rsl::unique_lock<rsl::mutex> lock(m_queue_mutex);
            if(!m_push_cv.wait_for(lock, waitDuration, [this] { return !this->m_q.empty(); }))
            {
              return false;
            }

            poppedItem = rsl::move(m_q.front());
            m_q.pop_front();
          }

          m_pop_cv.notify_one();
          return true;
        }

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

        s32 overrun_counter()
        {
          const rsl::unique_lock<rsl::mutex> lock(m_queue_mutex);
          return m_q.overrun_counter();
        }

        s32 size()
        {
          const rsl::unique_lock<rsl::mutex> lock(m_queue_mutex);
          return m_q.size();
        }

        void reset_overrun_counter()
        {
          const rsl::unique_lock<rsl::mutex> lock(m_queue_mutex);
          m_q.reset_overrun_counter();
        }

      private:
        rsl::mutex m_queue_mutex;
        rsl::condition_variable m_push_cv;
        rsl::condition_variable m_pop_cv;
        rsl::circular_q<T> m_q;
      };
    } // namespace details
  }   // namespace log
} // namespace rex
