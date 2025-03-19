#pragma once

#include "util/types.h"
#include "memory/allocators/heap_allocator_deleter.h"
#include <memory>

namespace ppp
{
    namespace memory
    {
        //-------------------------------------------------------------------------
        // tagged_placement_new:
        // Creates an object of type TTaggedHeapContainer using placement new on a custom heap.
        // The memory allocation is performed using a memory policy which can be customized via TMemoryPolicy.
        // A specific memory tag (defaulting to the container's default tag) is used during allocation.
        // Returns a unique_ptr with a custom allocator_deleter to ensure proper deallocation.
        template <typename TTaggedHeapContainer,
            typename TMemoryPolicy = default_tagged_memory_policy_t<TTaggedHeapContainer>,
            typename u32 tag = tags::default_memory_tag_v<TTaggedHeapContainer>,
            typename... Args>
        std::unique_ptr<TTaggedHeapContainer, allocator_deleter<TTaggedHeapContainer, memory::tagged_heap_allocator<TTaggedHeapContainer, TMemoryPolicy, tag>>> tagged_placement_new(Args&&... args)
        {
            // Retrieve the heap from the memory policy.
            auto heap = TMemoryPolicy::get_heap();

            // Allocate memory for TTaggedHeapContainer using the specified tag.
            void* p = heap->allocate(tag, sizeof(TTaggedHeapContainer));

            // Construct the object in the allocated memory using placement new.
            TTaggedHeapContainer* ptr = new (p) TTaggedHeapContainer(std::forward<Args>(args)...);

            // Return a unique_ptr that will use the custom allocator_deleter for proper deallocation.
            return std::unique_ptr<TTaggedHeapContainer, allocator_deleter<TTaggedHeapContainer, memory::tagged_heap_allocator<TTaggedHeapContainer, TMemoryPolicy, tag>>>(ptr);
        }

        //-------------------------------------------------------------------------
        // placement_new:
        // Creates an object of type TGenericHeapContainer using placement new on a custom heap.
        // Uses a default global memory policy to get the heap and allocates memory without a specific tag.
        // Returns a unique_ptr with a custom allocator_deleter for correct cleanup.
        template <typename TGenericHeapContainer,
            typename TMemoryPolicy = default_global_memory_policy_t<TGenericHeapContainer>,
            typename... Args>
        std::unique_ptr<TGenericHeapContainer, allocator_deleter<TGenericHeapContainer, memory::generic_heap_allocator<TGenericHeapContainer, TMemoryPolicy>>> placement_new(Args&&... args)
        {
            // Retrieve the heap from the global memory policy.
            auto heap = TMemoryPolicy::get_heap();

            // Allocate memory for TGenericHeapContainer without a tag.
            void* p = heap->allocate(sizeof(TGenericHeapContainer));

            // Construct the object in the allocated memory using placement new.
            TGenericHeapContainer* ptr = new (p) TGenericHeapContainer(std::forward<Args>(args)...);

            // Return a unique_ptr with a custom deleter that correctly frees the allocated memory.
            return std::unique_ptr<TGenericHeapContainer, allocator_deleter<TGenericHeapContainer, memory::generic_heap_allocator<TGenericHeapContainer, TMemoryPolicy>>>(ptr);
        }
    }
}
