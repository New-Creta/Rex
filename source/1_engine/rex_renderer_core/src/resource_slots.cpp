#include "rex_renderer_core/resource_slots.h"
#include "rex_renderer_core/log.h"

#include "rex_engine/diagnostics/assert.h"

namespace rex
{
    namespace renderer
    {
        //-------------------------------------------------------------------------
        ResourceSlots::~ResourceSlots()
        {
            delete[] m_flags;
            m_flags = nullptr;
        }

        //-------------------------------------------------------------------------
        void ResourceSlots::initialize(s32 num)
        {
            resize(num);
        }

        //-------------------------------------------------------------------------
        ResourceSlot ResourceSlots::next_slot()
        {
            for (s32 i = 0; i < m_flag_capacity; ++i)
            {
                if (!m_flags[i].test_and_set(rsl::memory_order_acquire))
                {
                    return ResourceSlot(i);
                }
            }

            REX_LOG(LogRendererCore, "No available resource slots, resizing resource slots: {} => {}", m_flag_capacity, m_flag_capacity * 2);

            s32 current_m_flag_capacity = m_flag_capacity; // The capacity of the array will be changed after the next call.

            resize(m_flag_capacity * 2);

            if (!m_flags[current_m_flag_capacity].test_and_set(rsl::memory_order_acquire))
            {
                return ResourceSlot(current_m_flag_capacity);
            }

            return ResourceSlot::make_invalid();
        }

        //-------------------------------------------------------------------------
        bool ResourceSlots::free_slot(s32 slot)
        {
            if (slot >= m_flag_capacity)
            {
                return false; // Slot is not in use or out of range.
            }

            return m_flags[slot].test_and_set(rsl::memory_order_release); // Slot was successfully freed.
        }

        //-------------------------------------------------------------------------
        void ResourceSlots::resize(s32 num)
        {
            if (num == m_flag_capacity)
            {
                return; // No op
            }

            REX_ASSERT_X(m_flag_capacity < num, "Shrinking the resource slots capacity is not supported");

            if (m_flags != nullptr)
            {
                delete[] m_flags;
                m_flags = nullptr;
            }

            s32 cur_cap = m_flag_capacity;
            s32 new_cap = num;

            rsl::atomic_flag* new_m_flags = new rsl::atomic_flag[num];

            for (s32 i = 0; i < cur_cap; ++i)
            {
                new_m_flags[i].test_and_set(rsl::memory_order_acquire);
            }
            for (s32 i = cur_cap; i < new_cap; ++i)
            {
                new_m_flags[i].clear(); // Initialize new m_flags as clear (FREE)
            }

            m_flags = new_m_flags;
            m_flag_capacity = num;
        }
    };
}