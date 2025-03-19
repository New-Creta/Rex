#include "memory/heaps/circular_heap.h"

#include "util/pointer_math.h"

namespace ppp
{
    namespace memory
    {
        //-------------------------------------------------------------------------
        circular_heap::circular_heap(iheap* heap, memory_size size)
            :m_total_size(size)
            ,m_base_memory(nullptr)
            ,m_tail(0)
            ,m_head(0)
        {
            if (size.size_in_bytes() > 0);
            {
                m_base_memory = heap->allocate(size);
            }
        }

        //-------------------------------------------------------------------------
        void* circular_heap::allocate(memory_size size) noexcept
        {
            assert(m_base_memory);

            constexpr u64 alignment = alignof(std::max_align_t);

            u64 aligned_head = align_up(m_head, alignment);
            u64 size_in_bytes = size.size_in_bytes();

            if (aligned_head + size_in_bytes > m_total_size)
            {
                aligned_head = align_up(0, alignment);

                m_head = 0;
            }

            // Check for overlap with the tail (oldest active allocation)
            // (The free space in the ring is non-contiguous; it is the region from head to tail.)
            if (!has_space(aligned_head, size_in_bytes)) 
            {
                // In a scratch allocator you sometimes don’t care about leaking over
                // because the allocations are temporary. Here we choose to simply reset
                // the entire allocator if there isn’t enough room.
                free();
                aligned_head = align_up(0, alignment);
            }

            void* ptr = ((u8*)m_base_memory) + aligned_head;
            
            // Advance head past the allocated block
            m_head = aligned_head + size_in_bytes;
            return ptr;
        }

        //-------------------------------------------------------------------------
        void circular_heap::deallocate(void* ptr) noexcept
        {
            assert(false && "Circular allocators should free all their allocated memory all at once");
        }

        //-------------------------------------------------------------------------
        void circular_heap::free() noexcept
        {
            if (m_base_memory != nullptr)
            {
                m_head = 0;
                m_tail = 0;
            }
        }

        //-------------------------------------------------------------------------
        bool circular_heap::can_alloc(memory_size size) const
        {
            constexpr u64 alignment = alignof(std::max_align_t);

            u64 aligned_head = align_up(m_head, alignment);
            u64 size_in_bytes = size.size_in_bytes();

            return has_space(aligned_head, size.size_in_bytes());
        }

        //-------------------------------------------------------------------------
        memory_size circular_heap::total_memory() const
        {
            return m_base_memory != nullptr ? m_total_size : 0;
        }

        //-------------------------------------------------------------------------
        memory_size circular_heap::current_memory() const
        {
            if (m_base_memory != nullptr)
            {
                u64 max = std::max(m_head, m_tail);
                u64 min = std::min(m_head, m_tail);

                return max - min;
            }

            return 0;
        }

        //-------------------------------------------------------------------------
        bool circular_heap::has_space(u64 allocation_start, u64 size) const
        {
            // There are two cases:
            // 1. head >= tail: the used region is from tail to head.
            //    The free space is from head to total_size, plus (if any) from 0 to tail.
            // 2. head < tail: free space is the contiguous block from head to tail.

            if (m_head >= m_tail)
            {
                // Free space from head to end
                u64 space_at_end = m_total_size.size_in_bytes() - allocation_start;

                // If the free block does not reach tail, it's safe.
                if (allocation_start >= m_tail && allocation_start + size > m_total_size.size_in_bytes())
                {
                    // Would wrap around and hit m_tail
                    return false;
                }

                return space_at_end >= size;
            }

            // head < tail: free space is [allocation_start, tail]
            return m_tail - allocation_start >= size;
        }
    }
}