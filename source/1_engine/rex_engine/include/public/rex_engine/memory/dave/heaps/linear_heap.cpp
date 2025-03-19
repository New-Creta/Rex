#include "memory/heaps/linear_heap.h"

#include "util/pointer_math.h"

namespace ppp
{
    namespace memory
    {
        //-------------------------------------------------------------------------
        linear_heap::linear_heap(iheap* heap, memory_size size)
            :m_total_memory(size)
            ,m_base_memory(nullptr)
            ,m_offset(0)
        {
            if (size.size_in_bytes() > 0)
            {
                m_base_memory = heap->allocate(size);
            }
        }

        //-------------------------------------------------------------------------
        void* linear_heap::allocate(memory_size size) noexcept
        {
            assert(m_base_memory);

            constexpr u64 alignment = alignof(std::max_align_t);

            uptr raw_address = reinterpret_cast<uptr>(m_base_memory) + m_offset;
            uptr aligned_address = align_up(raw_address, alignment);

            // Calculate the new offset after alignment
            u64 new_offset = aligned_address - reinterpret_cast<uptr>(m_base_memory) + size.size_in_bytes();

            // Assert if the allocation exceeds the available memory
            assert(new_offset <= m_total_memory.size_in_bytes() && "Out of memory!");

            m_offset = new_offset;

            // Return the aligned pointer
            return reinterpret_cast<void*>(aligned_address);
        }

        //-------------------------------------------------------------------------
        void linear_heap::deallocate(void* ptr) noexcept
        {
            assert(false && "Linear allocators should free all their allocated memory all at once");
        }

        //-------------------------------------------------------------------------
        void linear_heap::free() noexcept
        {
            if (m_base_memory != nullptr)
            {
                m_offset = 0;
            }
        }

        //-------------------------------------------------------------------------
        bool linear_heap::can_alloc(memory_size size) const
        {
            if (m_base_memory == nullptr)
            {
                return false;
            }

            constexpr std::size_t alignment = alignof(std::max_align_t);

            uptr raw_address = reinterpret_cast<uptr>(m_base_memory) + m_offset;
            uptr aligned_address = align_up(raw_address, alignment);

            // Calculate the required offset
            memory_size required_offset = static_cast<memory_size>(aligned_address - reinterpret_cast<uptr>(m_base_memory));

            // Check if the allocation fits within the remaining memory
            return required_offset + size <= m_total_memory;
        }

    }
}