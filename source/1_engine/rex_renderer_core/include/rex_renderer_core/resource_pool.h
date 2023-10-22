#pragma once

#include "rex_engine/types.h"

#include <rex_std/memory.h>

namespace rex
{
    namespace renderer
    {
        // lockless single producer multiple consumer - thread safe resource pool which will grow to accomodate contents
        template <typename T>
        class ResourcePool
        {
        public:
            ResourcePool();
            ~ResourcePool();

            void init(u32 reserved_capacity);
            void grow(u32 min_capacity);
            void insert(const T& resource, u32 slot);

            T& get(u32 slot);
            T& operator[](u32 slot);

        private:
            T* m_resources = nullptr;
            std::atomic<size_t> m_capacity;
        };

        template <typename T>
        ResourcePool<T>::ResourcePool()
        {
            m_capacity = 0;
        }

        template <typename T>
        ResourcePool<T>::~ResourcePool()
        {
            rex::memory_free(m_resources);
        }

        template <typename T>
        void ResourcePool<T>::init(u32 reserved_capacity)
        {
            m_capacity = reserved_capacity;
            m_resources = (T*)rex::memory_alloc(sizeof(T) * reserved_capacity);

            rsl::memset(m_resources, 0x0, sizeof(T) * m_capacity);
        }

        template <typename T>
        void ResourcePool<T>::grow(u32 min_capacity)
        {
            if (m_capacity <= min_capacity)
            {
                size_t new_cap = (min_capacity * 2);

                m_resources = (T*)rex::memory_realloc(m_resources, new_cap * sizeof(T));

                size_t existing_offset = m_capacity * sizeof(T);
                memset(((u8*)m_resources) + existing_offset, 0x0, sizeof(T) * m_capacity);

                m_capacity = new_cap;
            }
        }

        template <typename T>
        void ResourcePool<T>::insert(const T& resource, u32 slot)
        {
            grow(slot);
            memcpy(&m_resources[slot], &resource, sizeof(T));
        }

        template <typename T>
        T& ResourcePool<T>::get(u32 slot)
        {
            return m_resources[slot];
        }

        template <typename T>
        T& ResourcePool<T>::operator[](u32 slot)
        {
            return m_resources[slot];
        }
    }
}