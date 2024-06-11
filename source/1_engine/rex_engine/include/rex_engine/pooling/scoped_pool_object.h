#pragma once

#include "rex_std/functional.h"
#include "rex_std/utility.h"

namespace rex
{
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
    ScopedPoolObject(ScopedPoolObject&& other) = default;

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

    bool has_object() const
    {
      return m_object != nullptr;
    }
    void return_to_pool()
    {
      if (m_return_to_pool_callable)
      {
        m_return_to_pool_callable(m_object);
        m_object = nullptr;
        m_return_to_pool_callable = [](T*) {}; // do nothing
      }
    }

  private:
    return_to_pool_func m_return_to_pool_callable; // This callable is meant to return this object back to the pool it came from.
    T* m_object;
  };
}