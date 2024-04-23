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
 *  @file descriptor_allocator_page.h
 *  @date October 22, 2018
 *  @author Jeremiah van Oosten
 *  @brief A descriptor heap (page for the DescriptorAllocator class).
 * 
 *  Variable sized memory allocation strategy based on:
 *  http://diligentgraphics.com/diligent-engine/architecture/d3d12/variable-size-memory-allocations-manager/
 */

#include "rex_directx/directx_util.h"
#include "rex_directx/descriptors/descriptor_allocation.h"

#include "rex_std/map.h"
#include "rex_std/memory.h"
#include "rex_std/mutex.h"
#include "rex_std/queue.h"

#include <mutex>

namespace rex
{
  namespace renderer
  {
    class Device;

    class DescriptorAllocatorPage : public rsl::enable_shared_from_this<DescriptorAllocatorPage>
    {
    public:
      D3D12_DESCRIPTOR_HEAP_TYPE heap_type() const;

      /**
       * Check to see if this descriptor page has a contiguous block of descriptors
       * large enough to satisfy the request.
       */
      bool has_space(u32 numDescriptors) const;

      /**
       * Get the number of available handles in the heap.
       */
      u32 num_free_handles() const;

      /**
       * Allocate a number of descriptors from this descriptor heap.
       * If the allocation cannot be satisfied, then a NULL descriptor
       * is returned.
       */
      DescriptorAllocation allocate(u32 numDescriptors);

      /**
       * Return a descriptor back to the heap.
       * @param frameNumber Stale descriptors are not freed directly, but put
       * on a stale allocations queue. Stale allocations are returned to the heap
       * using the DescriptorAllocatorPage::ReleaseStaleAllocations method.
       */
      void free(DescriptorAllocation&& descriptorHandle);

      /**
       * Returned the stale descriptors back to the descriptor heap.
       */
      void release_stale_descriptors();

    protected:
      DescriptorAllocatorPage(Device& device, D3D12_DESCRIPTOR_HEAP_TYPE type, u32 numDescriptors);
      virtual ~DescriptorAllocatorPage();

    private:
      // Compute the offset of the descriptor handle from the start of the heap.
      u32 compute_offset(D3D12_CPU_DESCRIPTOR_HANDLE handle);

      // Adds a new block to the free list.
      void add_new_block(u32 offset, u32 numDescriptors);

      // Free a block of descriptors.
      // This will also merge free blocks in the free list to form larger blocks
      // that can be reused.
      void free_block(u32 offset, u32 numDescriptors);

    private:
      /*
       * The FreeListByOffset data structure stores a reference to the corresponding entry in the FreeListBySize map.
       * Similarly, each entry in the FreeListBySize map stores a reference by to the corresponding entry in the FreeListByOffset map.
       *
       * This solution resembles a bi-directional map (Bimap in Boost)
       *   which provides optimized searching on both offset and size of each entry in the free list.
       */

      // The offset (in descriptors) within the descriptor heap.
      using OffsetType = u32;
      // The number of descriptors that are available.
      using SizeType = u32;

      struct FreeBlockInfo;

      // A map that lists the free blocks by the offset within the descriptor heap.
      using FreeListByOffset = rsl::map<OffsetType, FreeBlockInfo>;
      // A map that lists the free blocks by size.
      // Needs to be a multimap since multiple blocks can have the same size.
      using FreeListBySize = rsl::multimap<SizeType, FreeListByOffset::iterator>;

      struct FreeBlockInfo
      {
        FreeBlockInfo(SizeType inSize)
            : size(inSize)
        {
        }

        SizeType size;
        FreeListBySize::iterator free_list_by_size_it;
      };

      FreeListByOffset m_free_list_by_offset;
      FreeListBySize m_free_list_by_size;

    private:
      struct StaleDescriptorInfo
      {
        StaleDescriptorInfo(OffsetType offsetType, SizeType sizeType)
            : offset(offsetType)
            , size(sizeType)
        {
        }

        // The offset within the descriptor heap.
        OffsetType offset;
        // The number of descriptors
        SizeType size;
      };

      // Stale descriptors are queued for release until the frame that they were freed
      // has completed.
      using StaleDescriptorQueue = rsl::queue<StaleDescriptorInfo>;

      StaleDescriptorQueue m_stale_descriptors;

    private:
      Device& m_device;
      wrl::ComPtr<ID3D12DescriptorHeap> m_d3d12_descriptor_heap;
      D3D12_DESCRIPTOR_HEAP_TYPE m_heap_type;
      CD3DX12_CPU_DESCRIPTOR_HANDLE m_base_descriptor;
      u32 m_descriptor_handle_increment_size;
      u32 m_num_descriptors_in_heap;
      u32 m_num_free_handles;

      rsl::mutex m_allocation_mutex;
    };
  } // namespace renderer
}