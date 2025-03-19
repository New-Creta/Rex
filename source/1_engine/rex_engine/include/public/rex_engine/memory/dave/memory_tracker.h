#pragma once

#include "util/types.h"

#include <unordered_map>
#include <mutex>
#include <memory>

namespace ppp
{
    namespace memory
    {
        void start_frame(s32 frame_index);
        void end_frame(bool dump_memory_info = false);

        bool enable_tracking();
        bool disable_tracking();

        bool is_tracking_enabled();

        void track_allocation(void* p, u64 size);
        void track_deallocation(void* p);

        void peek();
    }
}

//-------------------------------------------------------------------------
void* operator new(u64 size);
//-------------------------------------------------------------------------
void operator delete(void* p) noexcept;