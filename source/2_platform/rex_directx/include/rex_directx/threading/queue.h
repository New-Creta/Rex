#pragma once

#include "rex_std/mutex.h"
#include "rex_std/queue.h"

#include <mutex>
#include <queue>

namespace rex
{
  namespace renderer
  {
    namespace threading
    {
      template <typename T>
      class Queue
      {
      public:
        Queue();
        Queue(const Queue& copy);

        /**
         * push a value into the back of the queue.
         */
        void push(T value);

        /**
         * Try to pop a value from the front of the queue.
         * @returns false if the queue is empty.
         */
        bool try_pop(T& value);

        /**
         * Check to see if there are any items in the queue.
         */
        bool empty() const;

        /**
         * Retrieve the number of items in the queue.
         */
        size_t size() const;

      private:
        rsl::queue<T> m_queue;
        mutable rsl::mutex m_mutex;
      };

      template <typename T>
      Queue<T>::Queue()
      {
      }

      template <typename T>
      Queue<T>::Queue(const Queue<T>& copy)
      {
        rsl::unique_lock<rsl::mutex> lock(copy.m_mutex);
        m_queue = copy.m_queue;
      }

      template <typename T>
      void Queue<T>::push(T value)
      {
        rsl::unique_lock<rsl::mutex> lock(m_mutex);
        m_queue.push(rsl::move(value));
      }

      template <typename T>
      bool Queue<T>::try_pop(T& value)
      {
        rsl::unique_lock<rsl::mutex> lock(m_mutex);
        if(m_queue.empty())
          return false;

        value = m_queue.front();
        m_queue.pop();

        return true;
      }

      template <typename T>
      bool Queue<T>::empty() const
      {
        rsl::unique_lock<rsl::mutex> lock(m_mutex);
        return m_queue.empty();
      }

      template <typename T>
      size_t Queue<T>::size() const
      {
        rsl::unique_lock<rsl::mutex> lock(m_mutex);
        return m_queue.size();
      }
    } // namespace threading
  }   // namespace renderer
} // namespace rex