#pragma once

#include "memory/allocators/heap_allocator_deleter.h"
#include <memory>

namespace ppp
{
    namespace memory
    {
        //--------------------------------------------------------------
        // make_unique:
        // A factory function that creates an object of type T using a provided Allocator.
        // It allocates memory for one object, constructs the object using placement new,
        // and wraps the raw pointer in a std::unique_ptr with a custom deleter (allocator_deleter)
        // to ensure that the memory is properly released using the allocator when the unique_ptr goes out of scope.
        //
        // Template Parameters:
        // T         - The type of object to be created.
        // Allocator - The allocator type used to allocate and deallocate memory for T.
        // Args      - Constructor arguments for creating an instance of T.
        //
        // Returns:
        // A std::unique_ptr to the newly created object with a custom deleter that knows how to free
        // the allocated memory using the Allocator.
        template <typename T, typename Allocator, typename... Args>
        std::unique_ptr<T, allocator_deleter<T, Allocator>> make_unique(Args&&... args)
        {
            // Create an instance of the Allocator.
            Allocator alloc;

            // Allocate memory for one object of type T.
            T* raw_ptr = alloc.allocate(1);

            // Construct the object in the allocated memory using placement new.
            new (raw_ptr) T(std::forward<Args>(args)...);

            // Return a std::unique_ptr with a custom deleter that will properly deallocate the object.
            return std::unique_ptr<T, allocator_deleter<T, Allocator>>(raw_ptr, allocator_deleter<T, Allocator>{});
        }
    }
}
