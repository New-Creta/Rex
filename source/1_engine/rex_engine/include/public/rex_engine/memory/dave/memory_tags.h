#pragma once

#include "util/types.h"

namespace ppp
{
    namespace memory
    {
        namespace tags
        {
            // Predefined memory tag values represented as bit flags.
            // BIT(n) performs a left-shift operation (1 << n).

            static const u32 graphics = BIT(0);   // Tag for graphics-related memory allocations
            static const u32 fileio = BIT(1);     // Tag for file I/O related memory allocations
            static const u32 global = BIT(2);     // Default tag for general/global memory allocations
            static const u32 stage = BIT(3);      // Tag for staging memory allocations

            //--------------------------------------------------------------------------
            // Type Traits to Check for Memory Tag and Allocator Support
            //--------------------------------------------------------------------------

            // Trait to detect if a type T has a nested type 'memory_tag'.
            // By default, has_memory_tag<T> inherits from std::false_type.
            template<typename T, typename = void>
            struct has_memory_tag : std::false_type {};

            // Specialization: if T has a nested type 'memory_tag', this version is selected,
            // and the trait inherits from std::true_type.
            template<typename T>
            struct has_memory_tag<T, std::void_t<typename T::memory_tag>> : std::true_type {};

            // Convenience variable template for has_memory_tag.
            template<typename T>
            static constexpr bool has_memory_tag_v = has_memory_tag<T>::value;

            // Trait to detect if a type T has a nested type 'allocator_type'.
            // By default, has_allocator<T> inherits from std::false_type.
            template<typename T, typename = void>
            struct has_allocator : std::false_type {};

            // Specialization: if T has a nested type 'allocator_type', this version is used,
            // and the trait inherits from std::true_type.
            template<typename T>
            struct has_allocator<T, std::void_t<typename T::allocator_type>> : std::true_type {};

            // Convenience variable template for has_allocator.
            template<typename T>
            static constexpr bool has_allocator_v = has_allocator<T>::value;

            //--------------------------------------------------------------------------
            // Default Memory Tag Determination
            //--------------------------------------------------------------------------

            // Primary template for determining the default memory tag.
            // If no specialization applies, default to using the 'global' memory tag.
            template<typename, typename = void>
            struct default_memory_tag
            {
                static constexpr u32 memory_tag = tags::global;
            };

            // Specialization for types that define their own memory tag.
            // If T has a nested 'memory_tag', use that value as the default.
            template<typename T>
            struct default_memory_tag<T, std::enable_if_t<has_memory_tag_v<T>>>
            {
                static constexpr u32 memory_tag = T::memory_tag;
            };

            // Specialization for types that do not have a memory tag but provide an allocator.
            // In this case, use the memory tag defined by the allocator of T.
            template<typename T>
            struct default_memory_tag<T, std::enable_if_t<!has_memory_tag_v<T>&& has_allocator_v<T>>>
            {
                static constexpr u32 memory_tag = default_memory_tag<typename T::allocator_type>::memory_tag;
            };

            // Convenience variable template to extract the default memory tag value for a type T.
            template<typename T>
            static constexpr u32 default_memory_tag_v = default_memory_tag<T>::memory_tag;
        }
    }
}
