#pragma once

#include "rex_std/functional.h"
#include "rex_std/utility.h"

namespace rex
{
  // Wrapper around an object belonging to the pool
  // It'll automatically return it back to the pool when it goes out of scope
  // The pool is responsible of creating the callable to return this object back to itself.
  template <typename T>
  class ObjectWithDestructionCallback
  {
    using destruction_callback_func = rsl::function<void(T*)>;

  public:
    ObjectWithDestructionCallback()
      : m_object(nullptr)
      , m_destruction_callback()
    {}
    ObjectWithDestructionCallback(T* object, destruction_callback_func&& callable)
      : m_object(object)
      , m_destruction_callback(rsl::move(callable))
    {}

    ObjectWithDestructionCallback(const ObjectWithDestructionCallback&) = delete;
    ObjectWithDestructionCallback(ObjectWithDestructionCallback&& other)
    {
      m_object = rsl::exchange(other.m_object, nullptr);
      m_destruction_callback = rsl::move(other.m_destruction_callback);
    }

    ~ObjectWithDestructionCallback()
    {
      call_destruction_callback();
    }

    ObjectWithDestructionCallback& operator=(const ObjectWithDestructionCallback&) = delete;
    ObjectWithDestructionCallback& operator=(ObjectWithDestructionCallback&& other)
    {
      call_destruction_callback();
      m_object = rsl::exchange(other.m_object, nullptr);
      m_destruction_callback = rsl::move(other.m_destruction_callback);

      return *this;
    }

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
    ObjectWithDestructionCallback<U> convert()
    {
      U* u_ptr = static_cast<U*>(m_object);
      return ObjectWithDestructionCallback<U>(u_ptr, rsl::move(m_destruction_callback));
    }

    // Return if we still have an object.
    // We can lose an object after moving
    bool has_object() const
    {
      return m_object != nullptr;
    }

    // Call the destruction callback
    void call_destruction_callback()
    {
      if (has_object() && m_destruction_callback)
      {
        m_destruction_callback(m_object);
        clear();
      }
    }

  private:
    void clear()
    {
      m_object = nullptr;
      m_destruction_callback = [](T*) {}; // do nothing
    }

  private:
    destruction_callback_func m_destruction_callback; // This callable is meant to return this object back to the pool it came from.
    T* m_object;
  };
}