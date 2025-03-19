#pragma once

#include "memory/heaps/iheap.h"

#include "util/types.h"

namespace ppp
{
    namespace memory
    {
        class heap;

        class linear_heap : public iheap
        {
        public:
            linear_heap(iheap* heap, memory_size size);

        public:
            void*           allocate(memory_size size) noexcept override;
            void            deallocate(void* ptr) noexcept override;

            void            free() noexcept override;

        public:
            bool            can_alloc(memory_size size) const override;

            memory_size     total_memory() const override { return m_base_memory ? m_total_memory : 0; }
            memory_size     current_memory() const override { return m_base_memory ? m_offset : 0; }

        private:
            memory_size     m_total_memory;
            void*           m_base_memory;
            u64             m_offset;
        };
    }
}