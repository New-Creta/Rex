#pragma once

#include "rex_engine/types.h"
#include "rex_engine/threading/spin_lock.h"

#include <rex_std/memory.h>
#include <rex_std/vector.h>
#include <rex_std/atomic.h>

namespace rex
{
    namespace renderer
    {
        template <typename T>
        class ResourcePool
        {
        public:
            void initialize(s32 reservedCapacity);
            void validate_and_grow_if_necessary(s32 minCapacity);
            void insert(const T& resource, s32 slot);
            void clear();
            bool has_slot(s32 slot);

            T& operator[](s32 slot);
            const T& operator[](s32 slot) const;

        private:
            rsl::vector<T> m_resources;
            SpinLock m_lock;
        };

        template<typename U, typename T>
        U& get_resource_from_pool_as(ResourcePool<T>& pool, s32 slot)
        {
            return static_cast<U&>(*pool[slot]);
        }
        template<typename U, typename T>
        const U& get_resource_from_pool_as(const ResourcePool<T>& pool, s32 slot)
        {
            return static_cast<const U&>(pool[slot]);
        }

        //-----------------------------------------------------------------------
        template <typename T>
        void ResourcePool<T>::initialize(s32 reservedCapacity)
        {
            m_resources.reserve(reservedCapacity);
        }

        //-----------------------------------------------------------------------
        template <typename T>
        void ResourcePool<T>::validate_and_grow_if_necessary(s32 minCapacity)
        {
            m_lock.lock();
            if(minCapacity >= m_resources.capacity())
            {
                s32 new_cap = minCapacity * 2; // Grow to accommodate the slot
                m_resources.resize(new_cap);
            }
            m_lock.unlock();
        }

        //-----------------------------------------------------------------------
        template <typename T>
        void ResourcePool<T>::insert(const T& resource, s32 slot)
        {
            m_lock.lock();
            m_resources[slot] = resource;
            m_lock.unlock();
        }

        //-----------------------------------------------------------------------
        template <typename T>
        void ResourcePool<T>::clear()
        {
            m_lock.lock();
            m_resources.clear();
            m_lock.unlock();
        }

        //-----------------------------------------------------------------------
        template<typename T>
        bool ResourcePool<T>::has_slot(s32 slot)
        {
            return slot < m_resources.capacity();
        }

        //-----------------------------------------------------------------------
        template <typename T>
        T& ResourcePool<T>::operator[](s32 slot)
        {
            return m_resources[slot];
        }
        //-----------------------------------------------------------------------
        template <typename T>
        const T& ResourcePool<T>::operator[](s32 slot) const
        {
            return m_resources[slot];
        }
    }
}