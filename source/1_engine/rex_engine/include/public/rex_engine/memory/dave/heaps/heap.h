#pragma once

#include "memory/heaps/iheap.h"

#include "util/types.h"

namespace ppp
{
    namespace memory
    {
        class heap : public iheap
        {
        public:
            heap(memory_size size);
            ~heap();

            void* allocate(memory_size size) noexcept override;
            void deallocate(void* ptr) noexcept override;

            void free() noexcept override;

        public:
            bool can_alloc(memory_size size) const override;

            memory_size total_memory() const override { return m_base_memory ? m_total_size : 0; }
            memory_size current_memory() const override { return m_base_memory ? m_offset : 0; }

        private:
            u8* m_base_memory;
            u64 m_offset;

            memory_size m_total_size;
        };

        heap* get_heap();
    }
}