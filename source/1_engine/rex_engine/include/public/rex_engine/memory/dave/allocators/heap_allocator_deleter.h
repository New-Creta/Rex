#pragma once

#include <memory>

namespace ppp
{
    namespace memory
    {
        //--------------------------------------------------------------
        // Custom deleter that calls the destructor and then deallocates the memory using the provided Allocator.
        template <typename T, typename Allocator>
        struct allocator_deleter
        {
            void operator()(T* ptr) const
            {
                if (ptr)
                {
                    ptr->~T();

                    Allocator alloc;
                    alloc.deallocate(ptr, 1);
                }
            }

            // Conversion operator to allow converting to an allocator_deleter for another type U
            // with an arbitrary allocator OtherAllocator. This conversion is only enabled if:
            // - T* is convertible to U*, and
            // - Allocator is convertible to OtherAllocator.
            template <typename U, typename OtherAllocator>
            operator allocator_deleter<U, OtherAllocator>() const
            {
                static_assert(std::is_convertible<T*, U*>::value, "Pointer type conversion not allowed");
                static_assert(std::is_convertible<Allocator, OtherAllocator>::value, "Allocator type conversion not allowed");

                return allocator_deleter<U, OtherAllocator>();
            }
        };
    }
}