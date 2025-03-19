#pragma once

#include "memory/heaps/iheap.h"

namespace ppp
{
    namespace memory
    {
        template <typename T, typename TMemoryPolicy>
        class generic_heap_allocator
        {
        public:
            using value_type = T;
            using memory_policy = TMemoryPolicy;

            //-------------------------------------------------------------------------
            generic_heap_allocator() noexcept = default;

            //-------------------------------------------------------------------------
            template <typename U>
            generic_heap_allocator(const generic_heap_allocator<U, TMemoryPolicy>&) noexcept
            {
                // No state to copy since the allocator is stateless.
            }

            //-------------------------------------------------------------------------
            T* allocate(u64 n) noexcept
            {
                u64 total_size = n * sizeof(T);

                return static_cast<T*>(heap()->allocate(total_size));
            }

            //-------------------------------------------------------------------------
            void deallocate(T* p, u64 /*n*/) noexcept
            {
                // Will be cleared all at once
            }

            //-------------------------------------------------------------------------
            /*
             * The rebind template is required to allow the STL containers to use
             * this allocator with different types. For example, when a container
             * internally needs an allocator for a type different from T (such as its
             * internal node type), it uses the rebind mechanism to create an allocator
             * instance for that type.
             *
             * Without this template, the standard library's allocator_traits and
             * internal mechanisms (like _Replace_first_parameter) would fail to create
             * an allocator for the required type, resulting in compile-time errors.
             *
             * In short, the rebind mechanism ensures that your allocator is compatible
             * with the full range of operations expected by STL containers.
             */
            template <typename U>
            struct rebind
            {
                using other = generic_heap_allocator<U, TMemoryPolicy>;
            };

            iheap* heap() noexcept { return memory_policy::get_heap(); }
        };

        //-----------------------------------------------------------------------------
        // Equality operators
        // Standard-compliant allocators generally provide equality operators.
        //-----------------------------------------------------------------------------

        template<typename T, typename U, typename TMemoryPolicy>
        bool operator==(const generic_heap_allocator<T, TMemoryPolicy>&, const generic_heap_allocator<U, TMemoryPolicy>&) noexcept 
        {
            return true; // All instances are equivalent if they use the same policy.
        }

        template<typename T, typename U, typename TMemoryPolicy>
        bool operator!=(const generic_heap_allocator<T, TMemoryPolicy>& a, const generic_heap_allocator<U, TMemoryPolicy>& b) noexcept 
        {
            return !(a == b);
        }
    } // namespace memory
} // namespace ppp
