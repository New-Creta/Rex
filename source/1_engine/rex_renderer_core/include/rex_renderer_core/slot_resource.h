// slot_resource.h
// Copyright 2014 - 2019 Alex Dixon.
// License: https://github.com/polymonster/pmtech/blob/master/license.md

// Simple slot resource api can be used to allocate an array slot to a generic opaque resource via a handle.
// Implements a free list so getting a new resource slot is an o(1) operation.
// will grow to accomodate more items

#pragma once

#include "rex_engine/types.h"

namespace rex
{
    namespace renderer
    {
        struct FreeSlotList
        {
            u32 index;
            FreeSlotList* next;
            u32 flags;
        };

        struct SlotResources
        {
            FreeSlotList* slots;
            FreeSlotList* head;
            u32 capacity;
        };

        void slot_resources_grow(SlotResources* resources);
        void slot_resources_init(SlotResources* resources, u32 num);
        u32  slot_resources_get_next(SlotResources* resources);
        bool slot_resources_free(SlotResources* resources, const u32 slot);
    } // namespace renderer
} // namespace rex
