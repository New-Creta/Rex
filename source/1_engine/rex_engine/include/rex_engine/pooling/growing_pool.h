#pragma once

#include "rex_std/vector.h"
#include "rex_std/functional.h"
#include "rex_std/utility.h"

#include "rex_engine/engine/types.h"
#include "rex_engine/containers/vector_utils.h"
#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/pooling/scoped_pool_object.h"

namespace rex
{
  // The growing pool is a pool structure with the possibility to grow over time
  // An object can be requested, taking in an predicate to query for an idle object
  // If no idle object is found, a new one is constructed with the other parameters passed in
  // a function that's able to construct such an object is required in the constructor so
  // the pool can allocate such an object when there's no idle object found
  template <typename PooledObject, typename ... Args>
  class GrowingPool
  {
    // When looking for an idle object, we loop over all our idle objects and pass them into a function with the following signature
    // If it returns true, the object is free to take and put into action
    using find_obj_func = rsl::function<bool(const rsl::unique_ptr<PooledObject>&)>;

    // A function taking in variadic arguments and returning a unique pointer of the object we're pooling
    // They'll be added to the active objects immediateyly as you only want to create a new object if you need one
    using create_new_obj_func = rsl::function<rsl::unique_ptr<PooledObject>(Args&&...)>;

  public:
    GrowingPool()
      : m_allocate_new_object_func([](Args&& ... args) { return rsl::make_unique<PooledObject>(rsl::forward<Args>(args)...); })
    {}
    GrowingPool(create_new_obj_func&& createNewObjFunc)
      : m_allocate_new_object_func(rsl::move(createNewObjFunc))
    {}
    // Look for an idle object matching the predicate.
    // If no such object is found, a new one is constructed using the params passed in
    PooledObject* request(const find_obj_func& findIdleObjfunc, Args... args)
    {
      // First try to find if we have any idle object available for use, if so use one of those
      s32 free_obj_idx = find_free_object(findIdleObjfunc);
      if (free_obj_idx != -1)
      {
        return use_existing_object(free_obj_idx);
      }

      // If no idle object is available, create a new one
      return create_new_active_object(rsl::forward<Args>(args)...);
    }
    // Request an object that on destruction automatically returns itself to this pool
    ScopedPoolObject<PooledObject> request_scoped(const find_obj_func& findIdleObjfunc, Args... args)
    {
      PooledObject* obj = request(findIdleObjfunc, rsl::forward<Args>(args)...);
      return ScopedPoolObject<PooledObject>(obj,
        [this](PooledObject* pooledObj)
        {
          discard(pooledObj);
        });
    }

    // Return previously requested object back to the pool
    void discard(PooledObject* obj)
    {
      auto it = rsl::find_if(m_active_objects.begin(), m_active_objects.end(), [obj](const rsl::unique_ptr<PooledObject>& ptr) { return ptr.get() == obj; });
      REX_ASSERT_X(it != m_active_objects.end(), "Discarding object back to pool, but it doesn't belong to this pool");

      s32 idx = rsl::distance(m_active_objects.begin(), it);
      transfer_object_between_vectors(idx, m_active_objects, m_idle_objects);
    }

    // Return the number of idle objects
    s32 num_idle_objects() const
    {
      return m_idle_objects.size();
    }
    // Return the number of active objects
    s32 num_active_objects() const
    {
      return m_active_objects.size();
    }

    // Return the maximum number of supported active objects without reallocation
    s32 max_active_objects() const
    {
      return m_active_objects.capacity();
    }
    // Return the maximum number of supported idle objects without reallocation
    s32 max_idle_objects() const
    {
      return m_idle_objects.capacity();
    }

    // Resize the buffer holding idle objects to support the number objects given
    // If the number is smaller than the current number, a smaller buffer is allocated
    void resize(s32 newNumIdleObjects)
    {
      m_idle_objects.resize(newNumIdleObjects);
      m_active_objects.reserve(newNumIdleObjects);
    }

  private:
    // Find a free object matching the predicate in our idle pool
    s32 find_free_object(const find_obj_func& findIdleObjfunc)
    {
      // Look from last to front. If don't care which one we return, we prefer to return the last one
      auto it = rsl::find_if(m_idle_objects.crbegin(), m_idle_objects.crend(), findIdleObjfunc);
      if (it != m_idle_objects.crend())
      {
        return rsl::distance(m_idle_objects.cbegin(), it.base() - 1);
      }

      return -1;
    }
    // Retrieve an idle object, specified at the index, from the pool
    PooledObject* use_existing_object(s32 idx)
    {
      REX_ASSERT_X(idx != -1, "Invalid index used for pool");
      return rex::transfer_object_between_vectors(idx, m_idle_objects, m_active_objects).get();
    }
    // Create a new object and add it to the active objects
    template <typename ... Args>
    PooledObject* create_new_active_object(Args&&... args)
    {
      return m_active_objects.emplace_back(m_allocate_new_object_func(rsl::forward<Args>(args)...)).get();
    }

  private:
    rsl::vector<rsl::unique_ptr<PooledObject>> m_idle_objects;     // Holds all objects currently not in use
    rsl::vector<rsl::unique_ptr<PooledObject>> m_active_objects;   // Holds all objects currently in use
    create_new_obj_func m_allocate_new_object_func;
  };
}