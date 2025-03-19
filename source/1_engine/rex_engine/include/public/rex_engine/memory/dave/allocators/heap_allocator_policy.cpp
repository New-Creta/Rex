#include "memory/allocators/heap_allocator_policy.h"
#include "memory/memory_manager.h"

namespace ppp
{
    namespace memory
    {
        //-----------------------------------------------------------------------------
        // Persistent Memory Policy Implementations
        //-----------------------------------------------------------------------------

        /**
         * @brief Retrieves the heap for global persistent memory allocations.
         *
         * @return Pointer to the persistent global heap obtained from the memory manager.
         */
        iheap* persistent_global_policy::get_heap()
        {
            return get_memory_manager().get_persistent_region().get_heap();
        }

        /**
         * @brief Retrieves the heap for frame persistent memory allocations.
         *
         * @return Pointer to the frame persistent heap obtained from the memory manager.
         */
        iheap* persistent_frame_policy::get_heap()
        {
            return get_memory_manager().get_persistent_region().get_frame_heap();
        }

        /**
         * @brief Retrieves the heap for scratch persistent memory allocations.
         *
         * @return Pointer to the scratch persistent heap obtained from the memory manager.
         */
        iheap* persistent_scratch_policy::get_heap()
        {
            return get_memory_manager().get_persistent_region().get_scratch_heap();
        }

        /**
         * @brief Retrieves the tagged heap for persistent memory.
         *
         * @return Pointer to the tagged persistent heap obtained from the memory manager.
         */
        tagged_heap* persistent_tagged_policy::get_heap()
        {
            return get_memory_manager().get_persistent_region().get_tagged_heap();
        }

        //-----------------------------------------------------------------------------
        // Staging Memory Policy Implementations
        //-----------------------------------------------------------------------------

        /**
         * @brief Retrieves the heap for global staging memory allocations.
         *
         * @return Pointer to the global staging heap obtained from the memory manager.
         */
        iheap* transient_global_policy::get_heap()
        {
            return get_memory_manager().get_transient_region().get_heap();
        }

        /**
         * @brief Retrieves the heap for frame staging memory allocations.
         *
         * @return Pointer to the frame staging heap obtained from the memory manager.
         */
        iheap* transient_frame_policy::get_heap()
        {
            return get_memory_manager().get_transient_region().get_frame_heap();
        }

        /**
         * @brief Retrieves the heap for scratch staging memory allocations.
         *
         * @return Pointer to the scratch staging heap obtained from the memory manager.
         */
        iheap* transient_scratch_policy::get_heap()
        {
            return get_memory_manager().get_transient_region().get_scratch_heap();
        }

        /**
         * @brief Retrieves the tagged heap for staging memory.
         *
         * @return Pointer to the tagged staging heap obtained from the memory manager.
         */
        tagged_heap* transient_tagged_policy::get_heap()
        {
            return get_memory_manager().get_transient_region().get_tagged_heap();
        }

        //-----------------------------------------------------------------------------
        // Debug Memory Policy Implementations
        //-----------------------------------------------------------------------------

        /**
         * @brief Retrieves the heap for global debug memory allocations.
         *
         * @return Pointer to the global debug heap obtained from the memory manager.
         */
        iheap* debug_global_policy::get_heap()
        {
            return get_memory_manager().get_debug_region().get_heap();
        }

        /**
         * @brief Retrieves the heap for frame debug memory allocations.
         *
         * @return Pointer to the frame debug heap obtained from the memory manager.
         */
        iheap* debug_frame_policy::get_heap()
        {
            return get_memory_manager().get_debug_region().get_frame_heap();
        }

        /**
         * @brief Retrieves the heap for scratch debug memory allocations.
         *
         * @return Pointer to the scratch debug heap obtained from the memory manager.
         */
        iheap* debug_scratch_policy::get_heap()
        {
            return get_memory_manager().get_debug_region().get_scratch_heap();
        }

        /**
         * @brief Retrieves the tagged heap for debug memory.
         *
         * @return Pointer to the tagged debug heap obtained from the memory manager.
         */
        tagged_heap* debug_tagged_policy::get_heap()
        {
            return get_memory_manager().get_debug_region().get_tagged_heap();
        }
    }
}
