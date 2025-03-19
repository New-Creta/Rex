#include "memory/heaps/free_list_heap.h"

#include "util/pointer_math.h"

namespace ppp
{
    namespace memory
    {
        //-------------------------------------------------------------------------
        free_list_heap::free_list_heap(iheap* heap, memory_size size)
            : m_base_memory(nullptr)
            , m_free_list(nullptr)
            , m_total_memory_size(size)
        {
            if (size.size_in_bytes() > 0);
            {
                // Allocate a large contiguous block of memory from the heap.
                m_base_memory = static_cast<u8*>(heap->allocate(size));

                // Initialize the free list:
                // The entire pool is initially one large free block.
                m_free_list = reinterpret_cast<block_header*>(m_base_memory);
                m_free_list->size = size.size_in_bytes();
                m_free_list->next = nullptr;
            }
        }

        //-------------------------------------------------------------------------
        free_list_heap::~free_list_heap()
        {
            // No need to free m_base_memory here because its lifetime is managed by the heap.
        }

        //-------------------------------------------------------------------------
        void* free_list_heap::allocate(memory_size size) noexcept
        {
            assert(m_base_memory);
            assert(m_free_list);

            // Compute the total size needed.
            u64 total_size = calculate_total_size_to_allocate(size);

            block_header* prev = nullptr;
            block_header* curr = m_free_list;

            // Traverse free list to find a suitable block.
            while (curr != nullptr)
            {
                if (curr->size >= total_size)
                {
                    if (can_split_memory_block(curr, total_size))
                    {
                        split_memory_block(prev, curr, total_size);
                    }
                    else
                    {
                        remove_block_from_list(prev, curr);
                    }

                    return get_user_pointer(curr);
                }

                // Move to the next free block.
                prev = curr;
                curr = curr->next;
            }

            assert(false && "No suitable block found, perhaps ran out of available memory");
            return nullptr;
        }

        //-------------------------------------------------------------------------
        void free_list_heap::deallocate(void* ptr) noexcept
        {
            // Ignore null pointers.
            if (ptr == nullptr)
            {
                return;
            }

            assert(m_base_memory);
            assert(m_free_list);

            // Retrieve the block header (which is just before the user pointer).
            block_header* block = reinterpret_cast<block_header*>(reinterpret_cast<u8*>(ptr) - sizeof(block_header));

            // Insert the block into the free list and attempt to coalesce with neighbors.
            insert_and_coalesce(block);
        }

        //-------------------------------------------------------------------------
        void free_list_heap::free() noexcept
        {
            if (m_base_memory != nullptr && m_free_list != nullptr)
            {
                // Reinitialize the free list to cover the entire memory pool.
                m_free_list = reinterpret_cast<block_header*>(m_base_memory);
                m_free_list->size = m_total_memory_size.size_in_bytes();
                m_free_list->next = nullptr;
            }
        }

        //-------------------------------------------------------------------------
        bool free_list_heap::can_alloc(memory_size size) const
        {
            u64 total_size = calculate_total_size_to_allocate(size);

            for (block_header* curr = m_free_list; curr != nullptr; curr = curr->next) 
            {
                if (curr->size >= total_size) 
                {
                    return true;
                }
            }
            return false;
        }

        //-------------------------------------------------------------------------
        memory_size free_list_heap::current_memory() const
        {
            if (m_base_memory != nullptr && m_free_list != nullptr)
            {
                u64 free_memory = 0;
                for (block_header* curr = m_free_list; curr != nullptr; curr = curr->next)
                {
                    free_memory += curr->size;
                }

                // The used memory is total memory minus free memory.
                return m_total_memory_size.size_in_bytes() - free_memory;
            }

            return 0;
        }

        //-------------------------------------------------------------------------
        memory_size free_list_heap::total_memory() const
        {
            return m_base_memory != nullptr && m_free_list != nullptr ? m_total_memory_size : 0; 
        }

        //-------------------------------------------------------------------------
        // Returns the pointer for the user (just after the header).
        void* free_list_heap::get_user_pointer(block_header* block) const noexcept
        {
            return reinterpret_cast<void*>(reinterpret_cast<u8*>(block) + sizeof(block_header));
        }

        //-------------------------------------------------------------------------
        // Calculate the total size needed (requested size + header overhead) while ensuring proper alignment.
        u64 free_list_heap::calculate_total_size_to_allocate(memory_size size) const noexcept
        {
            constexpr u64 alignment = alignof(std::max_align_t);

            u64 requested_size = align_up(size.size_in_bytes(), alignment);

            return requested_size + sizeof(free_list_heap::block_header);
        }

        //-------------------------------------------------------------------------
        // Determine if a free block can be split after allocation.
        bool free_list_heap::can_split_memory_block(block_header* block, memory_size size) const noexcept
        {
            return block->size - size.size_in_bytes() > sizeof(block_header);
        }

        //-------------------------------------------------------------------------
        // Splits the current free block and updates the free list.
        void free_list_heap::split_memory_block(block_header* prev, block_header*& curr, memory_size size) noexcept
        {
            // Calculate address of the new free block.
            block_header* new_block = reinterpret_cast<block_header*>(reinterpret_cast<u8*>(curr) + size.size_in_bytes());
            new_block->size = curr->size - size.size_in_bytes();
            new_block->next = curr->next;

            // Update current block size to reflect the allocation.
            curr->size = size.size_in_bytes();

            // Update the free list pointer.
            if (prev == nullptr) 
            {
                m_free_list = new_block;
            }
            else 
            {
                prev->next = new_block;
            }
        }

        //-------------------------------------------------------------------------
        // Removes a block from the free list when it cannot be split.
        void free_list_heap::remove_block_from_list(block_header* prev, block_header* curr) noexcept
        {
            if (prev == nullptr) 
            {
                m_free_list = curr->next;
            }
            else 
            {
                prev->next = curr->next;
            }
        }

        //-------------------------------------------------------------------------
        // Insert a freed block back into the free list in sorted order,
        //      and then coalesce with adjacent free blocks.
        void free_list_heap::insert_and_coalesce(block_header* block) noexcept 
        {
            block_header** curr_ptr = &m_free_list;
            block_header* prev = nullptr;

            // Insert block in order (by address).
            while (*curr_ptr && *curr_ptr < block) 
            {
                prev = *curr_ptr;
                curr_ptr = &((*curr_ptr)->next);
            }

            block->next = *curr_ptr;
            *curr_ptr = block;

            // Coalesce with the previous block if adjacent.
            if (prev) 
            {
                u8* prev_end = reinterpret_cast<u8*>(prev) + prev->size;
                if (prev_end == reinterpret_cast<u8*>(block)) 
                {
                    prev->size += block->size;
                    prev->next = block->next;
                    block = prev; // Continue coalescing from the merged block.
                }
            }

            // Coalesce with the next block if adjacent.
            block_header* next = block->next;
            if (next) 
            {
                u8* block_end = reinterpret_cast<u8*>(block) + block->size;
                if (block_end == reinterpret_cast<u8*>(next)) 
                {
                    block->size += next->size;
                    block->next = next->next;
                }
            }
        }
    }
}