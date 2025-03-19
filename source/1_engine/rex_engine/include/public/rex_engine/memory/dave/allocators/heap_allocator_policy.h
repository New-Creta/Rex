/**
 * @file heap_allocator_policy.h
 * @brief Defines memory policies for persistent, staging, and debug memory allocations.
 *
 * This header provides several structures that define different memory policies
 * for obtaining heap instances from various memory regions. Each policy has a
 * static get_heap() function that returns a pointer to the associated heap.
 *
 * The policies are grouped into three main categories:
 *  - Persistent memory policies: for memory that persists across sessions.
 *  - Staging memory policies: for temporary memory used during staging.
 *  - Debug memory policies: for memory allocations used in debugging.
 *
 * Additionally, this file defines several template metafunctions to detect
 * if a type provides a custom memory policy or allocator type, as well as to
 * determine the default memory policy.
 */

#pragma once

#include <type_traits>

namespace ppp
{
    namespace memory
    {
        // Forward declarations for heap types.
        class iheap;
        class tagged_heap;

        //-----------------------------------------------------------------------------
        // Persistent Memory Policies
        //-----------------------------------------------------------------------------

        /**
         * @brief Global persistent memory policy.
         *
         * Provides access to the heap used for global persistent memory allocations.
         */
        struct persistent_global_policy
        {
            /**
             * @brief Retrieves the global persistent heap.
             * @return Pointer to the global persistent heap.
             */
            static iheap* get_heap();
        };

        /**
         * @brief Frame persistent memory policy.
         *
         * Provides access to the heap used for frame-level persistent memory allocations.
         */
        struct persistent_frame_policy
        {
            /**
             * @brief Retrieves the frame persistent heap.
             * @return Pointer to the frame persistent heap.
             */
            static iheap* get_heap();
        };

        /**
         * @brief Scratch persistent memory policy.
         *
         * Provides access to the heap used for scratch (temporary) persistent memory.
         */
        struct persistent_scratch_policy
        {
            /**
             * @brief Retrieves the scratch persistent heap.
             * @return Pointer to the scratch persistent heap.
             */
            static iheap* get_heap();
        };

        /**
         * @brief Tagged persistent memory policy.
         *
         * Provides access to a tagged heap for persistent memory, allowing for additional
         * metadata or categorization.
         */
        struct persistent_tagged_policy
        {
            /**
             * @brief Retrieves the tagged persistent heap.
             * @return Pointer to the tagged persistent heap.
             */
            static tagged_heap* get_heap();
        };

        //-----------------------------------------------------------------------------
        // Staging Memory Policies
        //-----------------------------------------------------------------------------

        /**
         * @brief Global staging memory policy.
         *
         * Provides access to the heap used for global staging memory allocations.
         */
        struct transient_global_policy
        {
            /**
             * @brief Retrieves the global staging heap.
             * @return Pointer to the global staging heap.
             */
            static iheap* get_heap();
        };

        /**
         * @brief Frame staging memory policy.
         *
         * Provides access to the heap used for frame-level staging memory allocations.
         */
        struct transient_frame_policy
        {
            /**
             * @brief Retrieves the frame staging heap.
             * @return Pointer to the frame staging heap.
             */
            static iheap* get_heap();
        };

        /**
         * @brief Scratch staging memory policy.
         *
         * Provides access to the heap used for scratch (temporary) staging memory.
         */
        struct transient_scratch_policy
        {
            /**
             * @brief Retrieves the scratch staging heap.
             * @return Pointer to the scratch staging heap.
             */
            static iheap* get_heap();
        };

        /**
         * @brief Tagged staging memory policy.
         *
         * Provides access to a tagged heap for staging memory, allowing for extra metadata.
         */
        struct transient_tagged_policy
        {
            /**
             * @brief Retrieves the tagged staging heap.
             * @return Pointer to the tagged staging heap.
             */
            static tagged_heap* get_heap();
        };

        //-----------------------------------------------------------------------------
        // Debug Memory Policies
        //-----------------------------------------------------------------------------

        /**
         * @brief Global debug memory policy.
         *
         * Provides access to the heap used for global debug memory allocations.
         */
        struct debug_global_policy
        {
            /**
             * @brief Retrieves the global debug heap.
             * @return Pointer to the global debug heap.
             */
            static iheap* get_heap();
        };

        /**
         * @brief Frame debug memory policy.
         *
         * Provides access to the heap used for frame-level debug memory allocations.
         */
        struct debug_frame_policy
        {
            /**
             * @brief Retrieves the frame debug heap.
             * @return Pointer to the frame debug heap.
             */
            static iheap* get_heap();
        };

        /**
         * @brief Scratch debug memory policy.
         *
         * Provides access to the heap used for scratch (temporary) debug memory.
         */
        struct debug_scratch_policy
        {
            /**
             * @brief Retrieves the scratch debug heap.
             * @return Pointer to the scratch debug heap.
             */
            static iheap* get_heap();
        };

        /**
         * @brief Tagged debug memory policy.
         *
         * Provides access to a tagged heap for debug memory, useful for extra debugging metadata.
         */
        struct debug_tagged_policy
        {
            /**
             * @brief Retrieves the tagged debug heap.
             * @return Pointer to the tagged debug heap.
             */
            static tagged_heap* get_heap();
        };

        //-----------------------------------------------------------------------------
        // Template Metafunctions for Memory Policies
        //-----------------------------------------------------------------------------

        /**
         * @brief Trait to check if type T defines a nested memory_policy typedef.
         */
        template<typename T, typename = void>
        struct has_memory_policy : std::false_type {};

        /**
         * @brief Specialization for types that provide a memory_policy typedef.
         */
        template<typename T>
        struct has_memory_policy<T, std::void_t<typename T::memory_policy>> : std::true_type {};

        /**
         * @brief Convenience variable template for checking memory policy.
         */
        template<typename T>
        static constexpr bool has_policy_v = has_memory_policy<T>::value;

        /**
         * @brief Trait to check if type T defines an allocator_type typedef.
         */
        template<typename T, typename = void>
        struct has_allocator : std::false_type {};

        /**
         * @brief Specialization for types that provide an allocator_type typedef.
         */
        template<typename T>
        struct has_allocator<T, std::void_t<typename T::allocator_type>> : std::true_type {};

        /**
         * @brief Convenience variable template for checking allocator type.
         */
        template<typename T>
        static constexpr bool has_allocator_v = has_allocator<T>::value;

        /**
         * @brief Determines the default memory policy for a type T.
         *
         * If T defines a memory_policy, that policy is used; otherwise, the provided
         * TDefaultPolicy is used.
         */
        template<typename, typename TDefaultPolicy, typename = void>
        struct default_memory_policy
        {
            using memory_policy = TDefaultPolicy;
        };

        /**
         * @brief Specialization for types that define their own memory_policy.
         */
        template<typename T, typename TDefaultPolicy>
        struct default_memory_policy<T, TDefaultPolicy, std::enable_if_t<has_policy_v<T>>>
        {
            using memory_policy = typename T::memory_policy;
        };

        /**
         * @brief Specialization for types that do not define a memory_policy but have an allocator_type.
         */
        template<typename T, typename TDefaultPolicy>
        struct default_memory_policy<T, TDefaultPolicy, std::enable_if_t<!has_policy_v<T>&& has_allocator_v<T>>>
        {
            using memory_policy = typename default_memory_policy<typename T::allocator_type, TDefaultPolicy>::memory_policy;
        };

        /**
         * @brief Alias for determining the default global memory policy for type T.
         */
        template<typename T>
        using default_global_memory_policy_t = typename default_memory_policy<T, persistent_global_policy>::memory_policy;

        /**
         * @brief Alias for determining the default tagged memory policy for type T.
         */
        template<typename T>
        using default_tagged_memory_policy_t = typename default_memory_policy<T, persistent_tagged_policy>::memory_policy;
    }
}
