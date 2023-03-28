

#pragma once

// multi producer-multi consumer blocking queue.
// enqueue(..) - will block until room found to put the new message.
// enqueue_nowait(..) - will return immediately with false if no room left in
// the queue.
// dequeue_for(..) - will block until the queue is not empty or timeout have
// passed.

#include <rexlog/details/circular_q.h>

#include "rex_std/condition_variable.h"
#include "rex_std/mutex.h"

namespace rexlog {
namespace details {

template<typename T>
class mpmc_blocking_queue
{
public:
    using item_type = T;
    explicit mpmc_blocking_queue(size_t max_items)
        : q_(max_items)
    {}

#ifndef __MINGW32__
    // try to enqueue and block if no room left
    void enqueue(T &&item)
    {
        {
            rsl::unique_lock<rsl::mutex> lock(queue_mutex_);
            pop_cv_.wait(lock, [this] { return !this->q_.full(); });
            q_.push_back(rsl::move(item));
        }
        push_cv_.notify_one();
    }

    // enqueue immediately. overrun oldest message in the queue if no room left.
    void enqueue_nowait(T &&item)
    {
        {
            rsl::unique_lock<rsl::mutex> lock(queue_mutex_);
            q_.push_back(rsl::move(item));
        }
        push_cv_.notify_one();
    }

    // dequeue with a timeout.
    // Return true, if succeeded dequeue item, false otherwise
    bool dequeue_for(T &popped_item, rsl::chrono::milliseconds wait_duration)
    {
        {
            rsl::unique_lock<rsl::mutex> lock(queue_mutex_);
            if (!push_cv_.wait_for(lock, wait_duration, [this] { return !this->q_.empty(); }))
            {
                return false;
            }
            popped_item = rsl::move(q_.front());
            q_.pop_front();
        }
        pop_cv_.notify_one();
        return true;
    }

    // blocking dequeue without a timeout.
    void dequeue(T &popped_item)
    {
        {
            rsl::unique_lock<rsl::mutex> lock(queue_mutex_);
            push_cv_.wait(lock, [this] { return !this->q_.empty(); });
            popped_item = rsl::move(q_.front());
            q_.pop_front();
        }
        pop_cv_.notify_one();
    }

#else
    // apparently mingw deadlocks if the mutex is released before cv.notify_one(),
    // so release the mutex at the very end each function.

    // try to enqueue and block if no room left
    void enqueue(T &&item)
    {
        rsl::unique_lock<rsl::mutex> lock(queue_mutex_);
        pop_cv_.wait(lock, [this] { return !this->q_.full(); });
        q_.push_back(rsl::move(item));
        push_cv_.notify_one();
    }

    // enqueue immediately. overrun oldest message in the queue if no room left.
    void enqueue_nowait(T &&item)
    {
        rsl::unique_lock<rsl::mutex> lock(queue_mutex_);
        q_.push_back(rsl::move(item));
        push_cv_.notify_one();
    }

    // dequeue with a timeout.
    // Return true, if succeeded dequeue item, false otherwise
    bool dequeue_for(T &popped_item, rsl::chrono::milliseconds wait_duration)
    {
        rsl::unique_lock<rsl::mutex> lock(queue_mutex_);
        if (!push_cv_.wait_for(lock, wait_duration, [this] { return !this->q_.empty(); }))
        {
            return false;
        }
        popped_item = rsl::move(q_.front());
        q_.pop_front();
        pop_cv_.notify_one();
        return true;
    }

    // blocking dequeue without a timeout.
    void dequeue(T &popped_item)
    {
        rsl::unique_lock<rsl::mutex> lock(queue_mutex_);
        push_cv_.wait(lock, [this] { return !this->q_.empty(); });
        popped_item = rsl::move(q_.front());
        q_.pop_front();
        pop_cv_.notify_one();
    }

#endif

    size_t overrun_counter()
    {
        rsl::unique_lock<rsl::mutex> lock(queue_mutex_);
        return q_.overrun_counter();
    }

    size_t size()
    {
        rsl::unique_lock<rsl::mutex> lock(queue_mutex_);
        return q_.size();
    }

    void reset_overrun_counter()
    {
        rsl::unique_lock<rsl::mutex> lock(queue_mutex_);
        q_.reset_overrun_counter();
    }

private:
    rsl::mutex queue_mutex_;
    rsl::condition_variable push_cv_;
    rsl::condition_variable pop_cv_;
    rexlog::details::circular_q<T> q_;
};
} // namespace details
} // namespace rexlog
