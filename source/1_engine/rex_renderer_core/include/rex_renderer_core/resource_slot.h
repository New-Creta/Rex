#pragma once 

#include "rex_engine/types.h"
#include "rex_engine/diagnostics/assert.h"

#include <rex_std/atomic.h>
#include <rex_std/functional.h>

namespace rex
{
    namespace renderer
    {
        class ResourceSlot
        {
        public:
            static ResourceSlot make_invalid();

            ResourceSlot();
            explicit ResourceSlot(s32 value);

            ResourceSlot(const ResourceSlot& other);
            ResourceSlot(ResourceSlot&& other) noexcept;

            ResourceSlot& operator=(const ResourceSlot& other);
            ResourceSlot& operator=(ResourceSlot&& other) noexcept;

            ~ResourceSlot();

            bool is_valid() const;

            s32 slot_id() const;

        private:
            s32 m_slot_id;
            s32* m_ref_count;
        };

    }
}