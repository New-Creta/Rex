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
            m_flags.reset();
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

            REX_WARN(LogRendererCore, "No available resource slots, resizing resource slots: {} => {}", m_flag_capacity, m_flag_capacity * 2);

            s32 current_flag_capacity = m_flag_capacity; // The capacity of the array will be changed after the next call.

            resize(m_flag_capacity * 2);

            if (!m_flags[current_flag_capacity].test_and_set(rsl::memory_order_acquire))
            {
                return ResourceSlot(current_flag_capacity);
            }

            return ResourceSlot::make_invalid();
        }

        //-------------------------------------------------------------------------
        bool ResourceSlots::free_slot(s32 slot)
        {
            if (slot == globals::g_invalid_slot_id)
            {
                REX_WARN(LogRendererCore, "Trying to release an invalid resource from ResourceSlots");
                return true; // Slot is invalid
            }

            if (slot >= m_flag_capacity)
            {
                REX_ERROR(LogRendererCore, "Trying to release a slot that is out of range of the allocated ResourceSlots");
                return false; // Slot is not in use or out of range.
            }

            if (m_flags[slot].test_and_set(rsl::memory_order_release))
            {
                return true;
            }

            REX_ERROR(LogRendererCore, "Unable to test and set ResourceSlot");
            return false; // Unable to remove slot
        }

        //-------------------------------------------------------------------------
        void ResourceSlots::free_slots()
        {
            // Reset all atomic flags to indicate all slots are free
            for (s32 i = 0; i < m_flag_capacity; ++i)
            {
                m_flags[i].clear();
            }
        }

        //-------------------------------------------------------------------------
        void ResourceSlots::resize(s32 num)
        {
            if (num == m_flag_capacity)
            {
                return; // No op
            }

            REX_ASSERT_X(m_flag_capacity < num, "Shrinking the resource slots capacity is not supported");

            if (m_flags)
            {
                m_flags.reset();
            }

            s32 cur_cap = m_flag_capacity;
            s32 new_cap = num;

            rsl::unique_array<rsl::atomic_flag> new_m_flags = rsl::make_unique<rsl::atomic_flag[]>(num);

            for (s32 i = 0; i < cur_cap; ++i)
            {
                new_m_flags[i].test_and_set(rsl::memory_order_acquire);
            }
            for (s32 i = cur_cap; i < new_cap; ++i)
            {
                new_m_flags[i].clear(); // Initialize new m_flags as clear (FREE)
            }

            m_flags = rsl::move(new_m_flags);
            m_flag_capacity = num;
        }

        //-------------------------------------------------------------------------
        ResourceSlots::Iterator ResourceSlots::begin()
        {
            rsl::count_t start = 0;
            while (start < m_flag_capacity && m_flags[start].test())
            {
                ++start;
            }

            return Iterator(m_flags.get(), m_flag_capacity, start);
        }

        //-------------------------------------------------------------------------
        ResourceSlots::Iterator ResourceSlots::end()
        {
            return Iterator(m_flags.get(), m_flag_capacity, m_flag_capacity);
        }

        //-------------------------------------------------------------------------
        ResourceSlots::ConstIterator ResourceSlots::cbegin() const
        {
            rsl::count_t start = 0;
            while (start < m_flag_capacity && m_flags[start].test())
            {
                ++start;
            }
            return ConstIterator(m_flags.get(), m_flag_capacity, start);
        }

        //-------------------------------------------------------------------------
        ResourceSlots::ConstIterator ResourceSlots::cend() const
        {
            return ConstIterator(m_flags.get(), m_flag_capacity, m_flag_capacity);
        }
    }
}