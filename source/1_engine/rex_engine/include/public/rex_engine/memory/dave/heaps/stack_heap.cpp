#include "memory/heaps/stack_heap.h"
#include "util/pointer_math.h"
#include <cassert>

namespace ppp
{
    namespace memory
    {
        //-------------------------------------------------------------------------
        stack_heap::stack_heap(iheap* heap, memory_size size)
            : m_total_memory(size)
            , m_base_memory(nullptr)
            , m_offset(0)
        {
            if (size.size_in_bytes() > 0)
            {
                m_base_memory = heap->allocate(size);
            }
        }

        //-------------------------------------------------------------------------
        void* stack_heap::allocate(memory_size size) noexcept
        {
            assert(m_base_memory);

            constexpr u64 alignment = alignof(std::max_align_t);
            const u64 header_size = sizeof(allocation_header);

            uptr raw_address = reinterpret_cast<uptr>(m_base_memory) + m_offset;
            // Align the current offset to ensure proper alignment for the header.
            uptr aligned_address = align_up(raw_address, alignment);
            // User data begins right after the header.
            uptr user_address = aligned_address + header_size;

            // Calculate the extra padding needed.
            // Total bytes needed for this allocation: padding + header + requested size.
            u64 padding = aligned_address - raw_address;
            u64 allocation_total = padding + header_size + size.size_in_bytes();

            // Check that we have enough memory.
            if (m_offset + allocation_total > m_total_memory.size_in_bytes())
            {
                assert(false && "Out of memory!");
                return nullptr;
            }

            // Store the previous offset in the header.
            allocation_header* header = reinterpret_cast<allocation_header*>(aligned_address);
            header->previous_offset = m_offset;

            m_offset += allocation_total;

            // Return the pointer to the user data.
            return reinterpret_cast<void*>(user_address);
        }

        //-------------------------------------------------------------------------
        void stack_heap::deallocate(void* ptr) noexcept
        {
            if (ptr == nullptr)
            {
                return;
            }

            // The header is located immediately before the user pointer.
            uptr user_address = reinterpret_cast<uptr>(ptr);
            uptr header_address = user_address - sizeof(allocation_header);
            allocation_header* header = reinterpret_cast<allocation_header*>(header_address);

            // Only the most recent allocation can be deallocated.
            // Restore the offset to the value saved in the header.
            m_offset = header->previous_offset;
        }

        //-------------------------------------------------------------------------
        void stack_heap::free() noexcept
        {
            // Reset the entire stack.
            m_offset = 0;
        }

        //-------------------------------------------------------------------------
        bool stack_heap::can_alloc(memory_size size) const
        {
            if (m_base_memory == nullptr)
            {
                return false;
            }

            constexpr u64 alignment = alignof(std::max_align_t);
            const u64 header_size = sizeof(allocation_header);

            uptr base = reinterpret_cast<uptr>(m_base_memory);

            uptr header_address = align_up(base + m_offset, alignment);
            uptr user_address = header_address + header_size;
            u64 padding = header_address - (base + m_offset);
            u64 allocation_total = padding + header_size + size.size_in_bytes();

            return (m_offset + allocation_total) <= m_total_memory.size_in_bytes();
        }
    }
}
