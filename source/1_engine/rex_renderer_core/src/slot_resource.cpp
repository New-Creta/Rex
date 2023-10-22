#include "rex_renderer_core/slot_resource.h"
#include "rex_engine/memory/memory_allocation.h"

namespace rex
{
    namespace renderer
    {
        enum ResourceFlags
        {
            RESOURCE_FREE = 1,
            RESOURCE_USED = 1 << 1
        };

        //-------------------------------------------------------------------------
        void slot_resources_grow(SlotResources* resources)
        {
            s32 cur_cap = resources->capacity;
            s32 new_cap = resources->capacity * 2;

            resources->slots = (FreeSlotList*)rex::memory_realloc(resources->slots, sizeof(FreeSlotList) * new_cap);

            // init free list
            for (s32 i = new_cap - 1; i > cur_cap; --i)
            {
                resources->slots[i].index = i;

                if (i >= new_cap - 1)
                {
                    resources->slots[i].next = nullptr;
                }
                else
                {
                    resources->slots[i].next = resources->head;
                }

                resources->head = &resources->slots[i];

                resources->slots[i].flags |= RESOURCE_FREE;
            }

            resources->capacity = new_cap;
        }

        //-------------------------------------------------------------------------
        void slot_resources_init(SlotResources* resources, u32 num)
        {
            resources->capacity = num;
            resources->slots = (FreeSlotList*)rex::memory_alloc(sizeof(SlotResources) * num);

            // 0 is reserved as null slot
            for (s32 i = num - 1; i > 0; --i)
            {
                resources->slots[i].index = i;

                if (i >= ((s32)num) - 1)
                {
                    resources->slots[i].next = nullptr;
                }
                else
                {
                    resources->slots[i].next = resources->head;
                }

                resources->head = &resources->slots[i];

                resources->slots[i].flags |= RESOURCE_FREE;
            }
        }

        //-------------------------------------------------------------------------
        u32 slot_resources_get_next(SlotResources* resources)
        {
            if (!resources->head)
            {
                slot_resources_grow(resources);
            }

            u32 r = resources->head->index;

            resources->head->flags &= ~RESOURCE_FREE;
            resources->head->flags |= RESOURCE_USED;

            resources->head = resources->head->next;

            return r;
        }

        //-------------------------------------------------------------------------
        bool slot_resources_free(SlotResources* resources, const u32 slot)
        {
            if (slot == 0)
            {
                return false;
            }

            // avoid double free
            if (resources->slots[slot].flags & RESOURCE_FREE)
            {
                return false;
            }

            // mark free and add to free list
            resources->slots[slot].flags |= RESOURCE_FREE;
            resources->slots[slot].next = resources->head;
            resources->head = &resources->slots[slot];

            return true;
        }
    }
}