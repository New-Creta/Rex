#pragma once

#include "rex_std/functional.h"
#include "rex_std/utility.h"

namespace rex
{
  // Wrapper around an object belonging to the pool
  // It'll automatically return it back to the pool when it goes out of scope
  // The pool is responsible of creating the callable to return this object back to itself.
  template <typename T>
  class ScopedPoolObject
  {
    using return_to_pool_func = rsl::function<void(T*)>;

  public:
    ScopedPoolObject() = default;
    ScopedPoolObject(T* object, return_to_pool_func&& callable)
      : m_object(object)
      , m_return_to_pool_callable(rsl::move(callable))
    {}

    ScopedPoolObject(const ScopedPoolObject&) = delete;
    ScopedPoolObject(ScopedPoolObject&& other)
    {
      m_object = rsl::exchange(other.m_object, nullptr);
      m_return_to_pool_callable = rsl::move(other.m_return_to_pool_callable);
    }

    ~ScopedPoolObject()
    {
      return_to_pool();
    }

    ScopedPoolObject& operator=(const ScopedPoolObject&) = delete;
    ScopedPoolObject& operator=(ScopedPoolObject&&) = default;

    T* operator->()
    {
      return m_object;
    }
    const T* operator->() const
    {
      return m_object;
    }
    T& operator*()
    {
      return *m_object;
    }
    const T& operator*() const
    {
      return *m_object;
    }

    T* get()
    {
      return m_object;
    }
    const T* get() const
    {
      return m_object;
    }

    // Converts this object into an object holding U instead of T
    // The new object is returned, this object becomes empty
    template <typename U>
    ScopedPoolObject<U> convert()
    {
      U* u_ptr = static_cast<U*>(m_object);
      return ScopedPoolObject<U>(u_ptr, rsl::move(m_return_to_pool_callable));
    }

    // Return if we still have an object.
    // We can lose an object after moving
    bool has_object() const
    {
      return m_object != nullptr;
    }
    // Return the wrapped object back to the pool it came from
    void return_to_pool()
    {
      if (has_object() && m_return_to_pool_callable)
      {
        m_return_to_pool_callable(m_object);
        clear();
      }
    }

  private:
    void clear()
    {
      m_object = nullptr;
      m_return_to_pool_callable = [](T*) {}; // do nothing
    }

  private:
    return_to_pool_func m_return_to_pool_callable; // This callable is meant to return this object back to the pool it came from.
    T* m_object;
  };
}