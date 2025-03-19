#pragma once

#include "memory/memory_size.h"

namespace ppp
{
    namespace memory
    {
        class iheap
        {
        public:
            virtual void*           allocate(memory_size size) noexcept = 0;
            virtual void            deallocate(void* ptr) noexcept = 0;

            virtual void            free() noexcept = 0;

        public:
            virtual bool            can_alloc(memory_size size) const = 0;

            virtual memory_size     total_memory() const = 0;
            virtual memory_size     current_memory() const = 0;
        };
    }
}