#pragma once

/*
 *  Copyright(c) 2018 Jeremiah van Oosten
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files(the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions :
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *  IN THE SOFTWARE.
 */

/**
 *  @file descriptor_allocation.h
 *  @date October 22, 2018
 *  @author Jeremiah van Oosten
 *  @brief A single allocation for the descriptor allocator.
 *
 *  Variable sized memory allocation strategy based on:
 *  http://diligentgraphics.com/diligent-engine/architecture/d3d12/variable-size-memory-allocations-manager/
 *  Date Accessed: May 9, 2018
 */

#include "rex_engine/engine/types.h"

#include "rex_directx/directx_util.h"

#include "rex_std/bonus/memory/memory_size.h"

#include <cstdint>

namespace rex
{
    namespace renderer
    {
        class DescriptorAllocatorPage;

        class DescriptorAllocation
        {
        public:
            // Creates a NULL descriptor
            DescriptorAllocation();
            DescriptorAllocation(D3D12_CPU_DESCRIPTOR_HANDLE descriptor, u32 numHandles, rsl::memory_size descriptorSize, rsl::shared_ptr<DescriptorAllocatorPage> page);

            // The destructor will automatically free the allocation.
            ~DescriptorAllocation();

            // Copies are not allowed.
            DescriptorAllocation(const DescriptorAllocation&) = delete;
            DescriptorAllocation& operator=(const DescriptorAllocation&) = delete;

            // Move is allowed.
            DescriptorAllocation(DescriptorAllocation&& allocation);
            DescriptorAllocation& operator=(DescriptorAllocation&& other);

            // Check if this a valid descriptor.
            bool is_null() const;
            bool is_valid() const;

            // Get a descriptor at a particular offset in the allocation.
            D3D12_CPU_DESCRIPTOR_HANDLE descriptor_handle(u32 offset = 0) const;

            // Get the number of (consecutive) handles for this allocation.
            u32 num_handles() const;

        private:
            // Get the heap that this allocation came from.
            rsl::shared_ptr<DescriptorAllocatorPage> descriptor_allocator_page() const;

            // Free the descriptor back to the heap it came from.
            void free();

        private:
            // The base descriptor.
            D3D12_CPU_DESCRIPTOR_HANDLE m_descriptor;
            // The number of descriptors in this allocation.
            u32 m_num_handles;
            // The offset to the next descriptor.
            rsl::memory_size m_descriptor_size;

            // A pointer back to the original page where this allocation came from.
            rsl::shared_ptr<DescriptorAllocatorPage> m_page;
        };
    }
}