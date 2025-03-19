#include "memory/heaps/tagged_heap_block.h"

#include "util/pointer_math.h"

namespace ppp
{
    namespace memory
    {
        //-------------------------------------------------------------------------
        tagged_heap_block::tagged_heap_block(iheap* heap, memory_size size)
            :m_tag(0)
            ,m_linear_heap(heap, size)
        {

        }

        //-------------------------------------------------------------------------
        u8* tagged_heap_block::allocate(s32 tag, memory_size size) noexcept
        {
            // Set block tag
            m_tag = tag;

            return static_cast<u8*>(m_linear_heap.allocate(size));
        }

        //-------------------------------------------------------------------------
        void tagged_heap_block::deallocate(void* ptr) noexcept
        {
            assert(false && "tagged heap blocks are instantly freed");
        }

        //-------------------------------------------------------------------------
        void tagged_heap_block::free()
        {
            m_tag = 0;
            m_linear_heap.free();
        }
    }
}