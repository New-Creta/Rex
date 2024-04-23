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
 *  @file dynamic_descriptor_heap.h
 *  @date October 22, 2018
 *  @author Jeremiah van Oosten
 *  @brief 
 * 
 *  The purpose of the DynamicDescriptorHeap class is to allocate GPU visible
 *  descriptors that are used for binding CBV, SRV, UAV, and Samplers to the GPU pipeline
 *  for rendering or compute invocations. This is necessary since the descriptors provided
 *  by the DescriptorAllocator class shown in the previous section are CPU visible and
 *  cannot be used to bind resources to the GPU rendering pipeline. The DynamicDescriptorHeap
 *  class provides a staging area for CPU visible descriptors that are committed to GPU
 *  visible descriptor heaps when a Draw or Dispatch method is invoked on the command list.
 */

#include "rex_engine/engine/types.h"

#include "rex_directx/directx_util.h"

#include <cstdint>

#include "rex_std/memory.h"
#include "rex_std/queue.h"
#include "rex_std/functional.h"

namespace rex
{
  namespace renderer
  {
    class CommandList;
    class RootSignature;
    class Device;

    class DynamicDescriptorHeap
    {
    public:
      DynamicDescriptorHeap(Device& device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, u32 numDescriptorsPerHeap = 1024);
      ~DynamicDescriptorHeap();

      /**
       * Stages a contiguous range of CPU visible descriptors.
       * Descriptors are not copied to the GPU visible descriptor heap until
       * the CommitStagedDescriptors function is called.
       */
      void stage_descriptors(u32 rootParameterIndex, u32 offset, u32 numDescriptors, const D3D12_CPU_DESCRIPTOR_HANDLE srcDescriptor);

      /**
       * Stage an inline CBV descriptor.
       */
      void stage_inline_CBV(u32 rootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS bufferLocation);

      /**
       * Stage an inline SRV descriptor.
       */
      void stage_inline_SRV(u32 rootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS bufferLocation);

      /**
       * Stage an inline UAV descriptor.
       */
      void stage_inline_UAV(u32 rootParamterIndex, D3D12_GPU_VIRTUAL_ADDRESS bufferLocation);

      /**
       * Copy all of the staged descriptors to the GPU visible descriptor heap and
       * bind the descriptor heap and the descriptor tables to the command list.
       * The passed-in function object is used to set the GPU visible descriptors
       * on the command list. Two possible functions are:
       *   * Before a draw    : ID3D12GraphicsCommandList::SetGraphicsRootDescriptorTable
       *   * Before a dispatch: ID3D12GraphicsCommandList::SetComputeRootDescriptorTable
       *
       * Since the DynamicDescriptorHeap can't know which function will be used, it must
       * be passed as an argument to the function.
       */
      void commit_staged_descriptors_for_draw(CommandList& commandList);
      void commit_staged_descriptors_for_dispatch(CommandList& commandList);

      /**
       * Copies a single CPU visible descriptor to a GPU visible descriptor heap.
       * This is useful for the
       *   * ID3D12GraphicsCommandList::ClearUnorderedAccessViewFloat
       *   * ID3D12GraphicsCommandList::ClearUnorderedAccessViewUint
       * methods which require both a CPU and GPU visible descriptors for a UAV
       * resource.
       *
       * @param commandList The command list is required in case the GPU visible
       * descriptor heap needs to be updated on the command list.
       * @param cpuDescriptor The CPU descriptor to copy into a GPU visible
       * descriptor heap.
       *
       * @return The GPU visible descriptor.
       */
      D3D12_GPU_DESCRIPTOR_HANDLE copy_descriptor(CommandList& comandList, D3D12_CPU_DESCRIPTOR_HANDLE cpuDescriptor);

      /**
       * Parse the root signature to determine which root parameters contain
       * descriptor tables and determine the number of descriptors needed for
       * each table.
       */
      void parse_root_signature(const rsl::shared_ptr<RootSignature>& rootSignature);

      /**
       * Reset used descriptors. This should only be done if any descriptors
       * that are being referenced by a command list has finished executing on the
       * command queue.
       */
      void reset();

    private:
      // Request a descriptor heap if one is available.
      wrl::ComPtr<ID3D12DescriptorHeap> request_descriptor_heap();
      // Create a new descriptor heap of no descriptor heap is available.
      wrl::ComPtr<ID3D12DescriptorHeap> create_descriptor_heap();

      // Compute the number of stale descriptors that need to be copied
      // to GPU visible descriptor heap.
      u32 compute_stale_descriptor_count() const;

      /**
       * Copy all of the staged descriptors to the GPU visible descriptor heap and
       * bind the descriptor heap and the descriptor tables to the command list.
       * The passed-in function object is used to set the GPU visible descriptors
       * on the command list. Two possible functions are:
       *   * Before a draw    : ID3D12GraphicsCommandList::SetGraphicsRootDescriptorTable
       *   * Before a dispatch: ID3D12GraphicsCommandList::SetComputeRootDescriptorTable
       *
       * Since the DynamicDescriptorHeap can't know which function will be used, it must
       * be passed as an argument to the function.
       */
      void commit_descriptor_tables(CommandList& commandList, rsl::function<void(ID3D12GraphicsCommandList*, u32, D3D12_GPU_DESCRIPTOR_HANDLE)> setFunc);
      void commit_inline_descriptors(CommandList& commandList, const D3D12_GPU_VIRTUAL_ADDRESS* bufferLocations, u32& bitMask, rsl::function<void(ID3D12GraphicsCommandList*, u32, D3D12_GPU_VIRTUAL_ADDRESS)> setFunc);

    private:
      /**
       * The maximum number of descriptor tables per root signature.
       * A 32-bit mask is used to keep track of the root parameter indices that
       * are descriptor tables.
       */
      static const u32 s_max_descriptor_tables = 32;

    private:
      /**
       * A structure that represents a descriptor table entry in the root signature.
       */
      struct DescriptorTableCache
      {
        DescriptorTableCache()
            : num_descriptors(0)
            , base_descriptor(nullptr)
        {
        }

        // Reset the table cache.
        void reset()
        {
          num_descriptors = 0;
          base_descriptor = nullptr;
        }

        // The number of descriptors in this descriptor table.
        u32 num_descriptors;
        // The pointer to the descriptor in the descriptor handle cache.
        D3D12_CPU_DESCRIPTOR_HANDLE* base_descriptor;
      };

      // The device that is used to create this descriptor heap.
      Device& m_device;

      // Describes the type of descriptors that can be staged using this
      // dynamic descriptor heap.
      // Valid values are:
      //   * D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
      //   * D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER
      // This parameter also determines the type of GPU visible descriptor heap to
      // create.
      D3D12_DESCRIPTOR_HEAP_TYPE m_descriptor_heap_type;

      // The number of descriptors to allocate in new GPU visible descriptor heaps.
      u32 m_num_descriptors_per_heap;

      // The increment size of a descriptor.
      u32 m_descriptor_handle_increment_size;

      // The descriptor handle cache.
      rsl::unique_array<D3D12_CPU_DESCRIPTOR_HANDLE> m_descriptor_handle_cache;

      // Descriptor handle cache per descriptor table.
      DescriptorTableCache m_descriptor_table_cache[s_max_descriptor_tables];

      // Inline CBV
      D3D12_GPU_VIRTUAL_ADDRESS m_inline_CBV[s_max_descriptor_tables];
      // Inline SRV
      D3D12_GPU_VIRTUAL_ADDRESS m_inline_SRV[s_max_descriptor_tables];
      // Inline UAV
      D3D12_GPU_VIRTUAL_ADDRESS m_inline_UAV[s_max_descriptor_tables];

      // Each bit in the bit mask represents the index in the root signature
      // that contains a descriptor table.
      u32 m_descriptor_table_bit_mask;

      // Each bit set in the bit mask represents a descriptor table
      // in the root signature that has changed since the last time the
      // descriptors were copied.
      u32 m_stale_descriptor_table_bit_mask;
      u32 m_stale_CBV_bit_mask;
      u32 m_stale_SRV_bit_mask;
      u32 m_stale_UAV_bit_mask;

      using DescriptorHeapPool = rsl::queue<wrl::ComPtr<ID3D12DescriptorHeap>>;

      DescriptorHeapPool m_descriptor_heap_pool;
      DescriptorHeapPool m_available_descriptor_heaps;

      wrl::ComPtr<ID3D12DescriptorHeap> m_current_descriptor_heap;
      CD3DX12_GPU_DESCRIPTOR_HANDLE m_current_GPU_descriptor_handle;
      CD3DX12_CPU_DESCRIPTOR_HANDLE m_current_CPU_descriptor_handle;

      u32 m_num_free_handles;
    };
  } // namespace renderer
}