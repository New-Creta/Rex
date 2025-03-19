#pragma once

#include "memory/heaps/iheap.h"
#include "memory/heaps/linear_heap.h"

namespace ppp
{
    namespace memory
    {
        class double_buffered_heap : public iheap
        {
        public:
            double_buffered_heap(iheap* heap, memory_size size);

        public:
            void*           allocate(memory_size size) noexcept override;
            void            deallocate(void* ptr) noexcept override;

            void            free() noexcept override;

        public:
            bool            can_alloc(memory_size size) const override;

            memory_size     total_memory() const override;
            memory_size     current_memory() const override;

        private:
            void            present();

        private:
            linear_heap*    m_active_linear_heap;

            linear_heap     m_buffer_0;
            linear_heap     m_buffer_1;
        };
    }
}