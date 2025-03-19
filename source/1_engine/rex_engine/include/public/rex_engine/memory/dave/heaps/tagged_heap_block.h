#pragma once

#include "util/types.h"

#include "memory/memory_size.h"
#include "memory/heaps/linear_heap.h"

namespace ppp
{
    namespace memory
    {
        class iheap;

        class tagged_heap_block
        {
        public:
            tagged_heap_block(iheap* heap, memory_size size);

            u8*             allocate(s32 tag, memory_size size) noexcept;
            void            deallocate(void* ptr) noexcept;

            void            free();

        public:
            u32             tag() const { return m_tag; }

            memory_size     current_size() const { return m_linear_heap.current_memory(); }
            memory_size     total_size() const { return m_linear_heap.total_memory(); }

            bool            can_alloc(memory_size size) const { return m_linear_heap.can_alloc(size); }

        private:
            u32             m_tag;
            linear_heap     m_linear_heap;
        };
    }
}