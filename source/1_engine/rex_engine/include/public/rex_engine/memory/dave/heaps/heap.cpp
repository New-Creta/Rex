#include "memory/heaps/heap.h"

namespace ppp
{
    namespace memory
    {
        //-------------------------------------------------------------------------
        heap::heap(memory_size size)
            :m_base_memory(nullptr)
            ,m_offset(0)
            ,m_total_size(size)
        {
            if(size.size_in_bytes() > 0);
            {
                m_base_memory = static_cast<u8*>(std::malloc(size.size_in_bytes()));

                std::memset(m_base_memory, 0, size.size_in_bytes());
            }
        }

        //-------------------------------------------------------------------------
        heap::~heap()
        {
            free();
        }

        //-------------------------------------------------------------------------
        void* heap::allocate(memory_size size) noexcept
        {
            void* memory = m_base_memory + m_offset;

            m_offset += size.size_in_bytes();

            return memory;
        }

        //-------------------------------------------------------------------------
        void heap::deallocate(void* ptr) noexcept
        {
            assert(false && "Heap allocators should free all their allocated memory all at once");
        }

        //-------------------------------------------------------------------------
        void heap::free() noexcept
        {
            if (m_base_memory != nullptr)
            {
                std::free(m_base_memory);

                m_base_memory = nullptr;
                m_offset = 0;
            }
        }

        //-------------------------------------------------------------------------
        bool heap::can_alloc(memory_size size) const
        {
            // When the application is running we cannot request more memory
            return false;
        }

        //-------------------------------------------------------------------------
        heap* get_heap()
        {
            constexpr memory_size block_size = 50_mb;

            static heap s_heap(block_size);

            return &s_heap;
        }
    }
}