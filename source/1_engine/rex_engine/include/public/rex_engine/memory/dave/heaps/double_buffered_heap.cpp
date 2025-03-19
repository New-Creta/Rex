#include "memory/heaps/double_buffered_heap.h"

namespace ppp
{
    namespace memory
    {
        //-------------------------------------------------------------------------
        double_buffered_heap::double_buffered_heap(iheap* heap, memory_size size)
            :m_active_linear_heap(nullptr)
            ,m_buffer_0(heap, size)
            ,m_buffer_1(heap, size)
        {
            m_active_linear_heap = &m_buffer_0;
        }

        //-------------------------------------------------------------------------
        void* double_buffered_heap::allocate(memory_size size) noexcept
        {
            return m_active_linear_heap->allocate(size);
        }

        //-------------------------------------------------------------------------
        void double_buffered_heap::deallocate(void* ptr) noexcept
        {
            m_active_linear_heap->deallocate(ptr);
        }

        //-------------------------------------------------------------------------
        void double_buffered_heap::free() noexcept
        {
            // Swap to a different buffer
            present();

            // Clear the new buffer, keeping the original available until the next free is called
            m_active_linear_heap->free();
        }

        //-------------------------------------------------------------------------
        bool double_buffered_heap::can_alloc(memory_size size) const
        {
            return m_active_linear_heap->can_alloc(size);
        }

        //-------------------------------------------------------------------------
        memory_size double_buffered_heap::total_memory() const
        {
            return m_buffer_0.total_memory() + m_buffer_1.total_memory();
        }

        //-------------------------------------------------------------------------
        memory_size double_buffered_heap::current_memory() const
        {
            return m_buffer_0.current_memory() + m_buffer_1.current_memory();
        }

        //-------------------------------------------------------------------------
        void double_buffered_heap::present()
        {
            m_active_linear_heap = m_active_linear_heap == &m_buffer_0
                ? &m_buffer_1
                : &m_buffer_0;
        }
    }
}