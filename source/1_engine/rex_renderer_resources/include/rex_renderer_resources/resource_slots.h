#pragma once

#include "rex_engine/engine/types.h"
#include "rex_renderer_resources/resource_slot.h"
#include "rex_std/atomic.h"
#include "rex_std/bonus/memory/unique_array.h"
#include "rex_std/vector.h"

namespace rex
{
  namespace renderer
  {
    /**
     * @class ResourceSlots
     * @brief Manages resource slots within the engine.
     *
     * This class is meant to allocate and free the various slots of resources that could exist within the engine.
     * It provides a mechanism to allocate and free slots for a variaty of resources.
     *
     * Resources are not directly exposed to the user of the API; instead, the user interacts with resource
     * indices provided by this class. The engine uses these indices to query the correct resource from
     * the resource pool using the RHI (Rendering Hardware Interface) layer.
     */
    class ResourceSlots
    {
    public:
      /**
       * @brief Iterator class for iterating over the allocated resource slots.
       *
       * This iterator allows traversal over the allocated resource slots.
       */
      class Iterator
      {
      public:
        // Iterator traits
        using iterator_category = rsl::forward_iterator_tag;
        using value_type        = ResourceSlot;
        using difference_type   = rsl::ptrdiff;
        using pointer           = ResourceSlot*;
        using reference         = ResourceSlot&;

        /**
         * @brief Constructs an iterator.
         *
         * Constructs an iterator for the resource slots with the provided flags, capacity, and current index.
         *
         * @param flags Pointer to the array of atomic flags.
         * @param capacity The capacity of the flags array.
         * @param current The current index of the iterator.
         */
        Iterator(rsl::atomic_flag* flags, rsl::count_t capacity, rsl::count_t current, rsl::function<bool(const ResourceSlot&)>* rendererReleaseFn)
            : m_flags(flags)
            , m_capacity(capacity)
            , m_current(current)
            , m_renderer_release_fn(rendererReleaseFn)
        {
        }

        /**
         * @brief Advances the iterator to the next valid resource slot.
         *
         * Increments the iterator to point to the next available resource slot.
         *
         * @return Reference to the updated iterator.
         */
        Iterator& operator++()
        {
          do
          {
            ++m_current;
          } while(m_current < m_capacity && m_flags[m_current].test());
          return *this;
        }

        /**
         * @brief Dereferences the iterator to obtain the current resource slot.
         *
         * Dereferences the iterator to retrieve the current resource slot.
         *
         * @return The current resource slot.
         */
        ResourceSlot operator*() const
        {
          return ResourceSlot(m_current, m_renderer_release_fn);
        }

        /**
         * @brief Checks for inequality between iterators.
         *
         * Checks if two iterators are not equal.
         *
         * @param other The iterator to compare against.
         * @return True if the iterators are not equal, false otherwise.
         */
        bool operator!=(const Iterator& other) const
        {
          return m_current != other.m_current;
        }

      private:
        rsl::atomic_flag* m_flags;
        s32 m_capacity;
        s32 m_current;
        rsl::function<bool(const ResourceSlot&)>* m_renderer_release_fn;
      };

      /**
       * @brief Const Iterator class for iterating over the allocated resource slots.
       *
       * This const iterator allows read-only traversal over the allocated resource slots.
       */
      class ConstIterator
      {
      public:
        // Iterator traits
        using iterator_category = rsl::forward_iterator_tag;
        using value_type        = const ResourceSlot;
        using difference_type   = rsl::ptrdiff;
        using pointer           = const ResourceSlot*;
        using reference         = const ResourceSlot&;

        /**
         * @brief Constructs an iterator.
         *
         * Constructs an iterator for the resource slots with the provided flags, capacity, and current index.
         *
         * @param flags Pointer to the array of atomic flags.
         * @param capacity The capacity of the flags array.
         * @param current The current index of the iterator.
         */
        ConstIterator(const rsl::atomic_flag* flags, rsl::count_t capacity, rsl::count_t current, const rsl::function<bool(const ResourceSlot&)>* rendererReleaseFn)
            : m_flags(flags)
            , m_capacity(capacity)
            , m_current(current)
            , m_renderer_release_fn(rendererReleaseFn)
        {
        }

        /**
         * @brief Advances the iterator to the next valid resource slot.
         *
         * Increments the iterator to point to the next available resource slot.
         *
         * @return Reference to the updated iterator.
         */
        ConstIterator& operator++()
        {
          do
          {
            ++m_current;
          } while(m_current < m_capacity && m_flags[m_current].test());
          return *this;
        }

        /**
         * @brief Dereferences the iterator to obtain the current resource slot.
         *
         * Dereferences the iterator to retrieve the current resource slot.
         *
         * @return The current resource slot.
         */
        ResourceSlot operator*() const
        {
          return ResourceSlot(m_current, m_renderer_release_fn);
        }

        /**
         * @brief Checks for inequality between iterators.
         *
         * Checks if two iterators are not equal.
         *
         * @param other The iterator to compare against.
         * @return True if the iterators are not equal, false otherwise.
         */
        bool operator!=(const ConstIterator& other) const
        {
          return m_current != other.m_current;
        }

      private:
        const rsl::atomic_flag* m_flags;
        s32 m_capacity;
        s32 m_current;
        const rsl::function<bool(const ResourceSlot&)>* m_renderer_release_fn;
      };

    public:
      explicit ResourceSlots(s32 num, const rsl::function<bool(const ResourceSlot&)>& rendererReleaseFn);
      ~ResourceSlots();

      /**
       * @brief Allocate the next available resource slot.
       *
       * @return The index of the allocated resource slot.
       */
      ResourceSlot alloc_slot();

      /**
       * @brief Free a previously allocated resource slot.
       *
       * @param slot The index of the resource slot to be freed.
       * @return `true` if the slot was successfully freed, `false` if the slot is not in use or out of range.
       */
      bool free_slot(s32 slot);

      /**
       * @brief Free all allocated resource slots.
       */
      void free_slots();

      /**
       * @brief Returns an iterator pointing to the beginning of the allocated resource slots.
       *
       * This function retrieves an iterator pointing to the first allocated resource slot.
       *
       * @return An iterator pointing to the beginning of the allocated resource slots.
       */
      Iterator begin();

      /**
       * @brief Returns an iterator pointing to the end of the allocated resource slots.
       *
       * This function retrieves an iterator pointing to the end of the allocated resource slots.
       *
       * @return An iterator pointing to the end of the allocated resource slots.
       */
      Iterator end();

      /**
       * @brief Returns an iterator pointing to the beginning of the allocated resource slots.
       *
       * This function retrieves an iterator pointing to the first allocated resource slot.
       *
       * @return An iterator pointing to the beginning of the allocated resource slots.
       */
      ConstIterator cbegin() const;

      /**
       * @brief Returns an iterator pointing to the end of the allocated resource slots.
       *
       * This function retrieves an iterator pointing to the end of the allocated resource slots.
       *
       * @return An iterator pointing to the end of the allocated resource slots.
       */
      ConstIterator cend() const;

    private:
      /**
       * Resize the internal array of atomic flags to accommodate a new number of resource slots.
       *
       * @param num The new number of resource slots to support.
       */
      void resize(s32 num);

    private:
      // An array of atomic flags to manage resource slot states.
      rsl::unique_array<rsl::atomic_flag> m_flags;
      rsl::count_t m_flag_capacity;
      rsl::function<bool(const ResourceSlot&)> m_renderer_release_fn;
    };
  } // namespace renderer
} // namespace rex