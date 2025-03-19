#pragma once

#include "memory/heaps/iheap.h"

#include "util/types.h"

namespace ppp
{
    namespace memory
    {
        class free_list_heap : public iheap
        {
        public:
            free_list_heap(iheap* heap, memory_size size);
            ~free_list_heap();

            void*           allocate(memory_size size) noexcept override;
            void            deallocate(void* ptr) noexcept override;

            void            free() noexcept override;

        public:
            bool            can_alloc(memory_size size) const override;

            memory_size     total_memory() const override;
            memory_size     current_memory() const override;

        private:
            struct block_header
            {
                u64 size;
                block_header* next;
            };

            u8*             m_base_memory;
            block_header*   m_free_list;
            memory_size     m_total_memory_size;

        private:
            void*           get_user_pointer(block_header* block) const noexcept;
            u64             calculate_total_size_to_allocate(memory_size size) const noexcept;
            bool            can_split_memory_block(block_header* block, memory_size size) const noexcept;
            void            split_memory_block(block_header* prev, block_header*& curr, memory_size size) noexcept;
            void            remove_block_from_list(block_header* prev, block_header* curr) noexcept;
            void            insert_and_coalesce(block_header* block) noexcept;
        };
    }
}