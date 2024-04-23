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
 *  @file descriptor_allocator.h
 *  @date October 22, 2018
 *  @author Jeremiah van Oosten
 *  @brief
 *   
 *  The DescriptorAllocator class is used to allocate descriptors to the application when loading new resources (like textures). 
 *  In a typical game engine, resources may need to be loaded and unloaded from memory at sporadic moments while the player moves around the level.
 *  To support large dynamic worlds, it may be necessary to initially load some resources, unload them from memory, and reload different resources.
 *  The DescriptorAllocator manages all of the descriptors that are required to describe those resources. 
 *  Descriptors that are no longer used (for example, when a resource is unloaded from memory) will be automatically returned back to the descriptor heap for reuse.
 */

#include "rex_engine/engine/types.h"

#include "rex_directx/directx_util.h"
#include "rex_directx/descriptors/descriptor_allocation.h"

#include <cstdint>

#include "rex_std/mutex.h"
#include "rex_std/memory.h"
#include "rex_std/set.h"
#include "rex_std/vector.h"

namespace rex
{
  namespace renderer
  {
    class Device;
    class DescriptorAllocatorPage;

    class DescriptorAllocator
    {
    public:
      /**
       * Allocate a number of contiguous descriptors from a CPU visible descriptor heap.
       *
       * @param numDescriptors The number of contiguous descriptors to allocate.
       * Cannot be more than the number of descriptors per descriptor heap.
       */
      DescriptorAllocation allocate(u32 numDescriptors = 1);

      /**
       * When the frame has completed, the stale descriptors can be released.
       */
      void release_stale_descriptors();

    protected:
      friend struct rsl::default_delete<DescriptorAllocator>;

      // Can only be created by the Device.
      DescriptorAllocator(Device& device, D3D12_DESCRIPTOR_HEAP_TYPE type, u32 numDescriptorsPerHeap = 256);
      virtual ~DescriptorAllocator();

    private:
      // Create a new heap with a specific number of descriptors.
      rsl::shared_ptr<DescriptorAllocatorPage> create_allocator_page();

    private:
      using DescriptorHeapPool = rsl::vector<rsl::shared_ptr<DescriptorAllocatorPage>>;

      Device& m_device;
      D3D12_DESCRIPTOR_HEAP_TYPE m_heap_type;
      u32 m_num_descriptors_per_heap;

      DescriptorHeapPool m_heap_pool;
      // Indices of available heaps in the heap pool.
      rsl::set<size_t> m_available_heaps;

      rsl::mutex m_allocation_mutex;
    };
  } // namespace renderer
}