#include "rex_renderer_core/resource_slots.h"
#include "rex_renderer_core/log.h"

#include "rex_engine/diagnostics/assert.h"

namespace rex
{
    namespace renderer
    {
        //-------------------------------------------------------------------------
        void ResourceSlots::initialize(u32 num)
        {
            resize(num);
        }

        //-------------------------------------------------------------------------
        u32 ResourceSlots::next_slot()
        {
            for (u32 i = 0; i < flags.capacity(); ++i)
            {
                if (!flags[i].test_and_set(std::memory_order_acquire))
                {
                    return i;
                }
            }

            REX_LOG(LogRendererCore, "No available resource slots, resizing resource slots: {} => {}", flags.capacity(), flags.capacity() * 2);

            resize(flags.capacity() * 2);

            return flags.size();
        }

        //-------------------------------------------------------------------------
        bool ResourceSlots::free_slot(u32 slot)
        {
            if (slot >= flags.capacity())
            {
                return false; // Slot is not in use or out of range.
            }

            return flags[slot].test_and_set(std::memory_order_release); // Slot was successfully freed.
        }

        //-------------------------------------------------------------------------
        void ResourceSlots::resize(u32 num)
        {
            if (num == flags.capacity())
            {
                return; // No op
            }

            REX_ASSERT_X(num > flags.capacity(), "Shrinking the resource slots capacity is not supported");

            u32 cur_cap = flags.capacity();
            u32 new_cap = num;

            flags.resize(num);

            for (u32 i = cur_cap; i < new_cap; ++i)
            {
                flags[i].clear(); // Initialize new flags as clear (FREE)
            }
        }
    };
}
}