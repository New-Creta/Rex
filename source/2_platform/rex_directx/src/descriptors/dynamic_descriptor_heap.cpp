#include "rex_directx/descriptors/dynamic_descriptor_heap.h"

#include "rex_directx/command_list.h"
#include "rex_directx/directx_call.h"
#include "rex_directx/directx_device.h"
#include "rex_directx/resources/root_signature.h"

#include "rex_directx/log.h"
#include "rex_engine/diagnostics/assert.h"

namespace rex
{
  namespace renderer
  {
    DynamicDescriptorHeap::DynamicDescriptorHeap(Device& device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, u32 numDescriptorsPerHeap)
        : m_device(device)
        , m_descriptor_heap_type(heapType)
        , m_num_descriptors_per_heap(numDescriptorsPerHeap)
        , m_descriptor_table_bit_mask(0)
        , m_stale_descriptor_table_bit_mask(0)
        , m_stale_CBV_bit_mask(0)
        , m_stale_SRV_bit_mask(0)
        , m_stale_UAV_bit_mask(0)
        , m_current_CPU_descriptor_handle(D3D12_DEFAULT)
        , m_current_GPU_descriptor_handle(D3D12_DEFAULT)
        , m_num_free_handles(0)
    {
      m_descriptor_handle_increment_size = device.d3d_device()->GetDescriptorHandleIncrementSize(heapType);

      // Allocate space for staging CPU visible descriptors.
      m_descriptor_handle_cache = rsl::make_unique<D3D12_CPU_DESCRIPTOR_HANDLE[]>(m_num_descriptors_per_heap);
    }

    DynamicDescriptorHeap::~DynamicDescriptorHeap() {}

    void DynamicDescriptorHeap::stage_descriptors(u32 rootParameterIndex, u32 offset, u32 numDescriptors, const D3D12_CPU_DESCRIPTOR_HANDLE srcDescriptor)
    {
      // Cannot stage more than the maximum number of descriptors per heap.
      // Cannot stage more than s_max_descriptor_tables root parameters.
      REX_ASSERT_X(numDescriptors <= m_num_descriptors_per_heap && rootParameterIndex < s_max_descriptor_tables, "Cannot stage more than the maximum number of descriptor heaps");

      DescriptorTableCache& descriptor_table_cache = m_descriptor_table_cache[rootParameterIndex];

      // Check that the number of descriptors to copy does not exceed the number of descriptors expected in the descriptor table.
      REX_ASSERT_X((offset + numDescriptors) <= descriptor_table_cache.num_descriptors, "Number of descriptors exceeds the number of descriptors in the descriptor table.");

      D3D12_CPU_DESCRIPTOR_HANDLE* dst_descriptor = (descriptor_table_cache.base_descriptor + offset);
      for(u32 i = 0; i < numDescriptors; ++i)
      {
        dst_descriptor[i] = CD3DX12_CPU_DESCRIPTOR_HANDLE(srcDescriptor, i, m_descriptor_handle_increment_size);
      }

      // Set the root parameter index bit to make sure the descriptor table
      // at that index is bound to the command list.
      m_stale_descriptor_table_bit_mask |= (1 << rootParameterIndex);
    }

    void DynamicDescriptorHeap::stage_inline_CBV(u32 rootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS bufferLocation)
    {
      REX_ASSERT_X(rootParameterIndex < s_max_descriptor_tables, "Root parameter index excees max descriptor tables");

      m_inline_CBV[rootParameterIndex] = bufferLocation;
      m_stale_CBV_bit_mask |= (1 << rootParameterIndex);
    }

    void DynamicDescriptorHeap::stage_inline_SRV(u32 rootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS bufferLocation)
    {
      REX_ASSERT_X(rootParameterIndex < s_max_descriptor_tables, "Root parameter index excees max descriptor tables");

      m_inline_SRV[rootParameterIndex] = bufferLocation;
      m_stale_SRV_bit_mask |= (1 << rootParameterIndex);
    }

    void DynamicDescriptorHeap::stage_inline_UAV(u32 rootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS bufferLocation)
    {
      REX_ASSERT_X(rootParameterIndex < s_max_descriptor_tables, "Root parameter index excees max descriptor tables");

      m_inline_UAV[rootParameterIndex] = bufferLocation;
      m_stale_UAV_bit_mask |= (1 << rootParameterIndex);
    }

    void DynamicDescriptorHeap::commit_staged_descriptors_for_draw(CommandList& commandList)
    {
      commit_descriptor_tables(commandList, [](ID3D12GraphicsCommandList* cmdList, u32 rootIndex, D3D12_GPU_DESCRIPTOR_HANDLE handle) { cmdList->SetGraphicsRootDescriptorTable(rootIndex, handle); });

      commit_inline_descriptors(commandList, m_inline_CBV, m_stale_CBV_bit_mask, [](ID3D12GraphicsCommandList* cmdList, u32 rootIndex, D3D12_GPU_VIRTUAL_ADDRESS address) { cmdList->SetGraphicsRootConstantBufferView(rootIndex, address); });
      commit_inline_descriptors(commandList, m_inline_SRV, m_stale_SRV_bit_mask, [](ID3D12GraphicsCommandList* cmdList, u32 rootIndex, D3D12_GPU_VIRTUAL_ADDRESS address) { cmdList->SetGraphicsRootShaderResourceView(rootIndex, address); });
      commit_inline_descriptors(commandList, m_inline_UAV, m_stale_UAV_bit_mask, [](ID3D12GraphicsCommandList* cmdList, u32 rootIndex, D3D12_GPU_VIRTUAL_ADDRESS address) { cmdList->SetGraphicsRootUnorderedAccessView(rootIndex, address); });
    }

    void DynamicDescriptorHeap::commit_staged_descriptors_for_dispatch(CommandList& commandList)
    {
      commit_descriptor_tables(commandList, [](ID3D12GraphicsCommandList* cmdList, u32 rootIndex, D3D12_GPU_DESCRIPTOR_HANDLE handle) { cmdList->SetComputeRootDescriptorTable(rootIndex, handle); });

      commit_inline_descriptors(commandList, m_inline_CBV, m_stale_CBV_bit_mask, [](ID3D12GraphicsCommandList* cmdList, u32 rootIndex, D3D12_GPU_VIRTUAL_ADDRESS address) { cmdList->SetComputeRootConstantBufferView(rootIndex, address); });
      commit_inline_descriptors(commandList, m_inline_SRV, m_stale_SRV_bit_mask, [](ID3D12GraphicsCommandList* cmdList, u32 rootIndex, D3D12_GPU_VIRTUAL_ADDRESS address) { cmdList->SetComputeRootShaderResourceView(rootIndex, address); });
      commit_inline_descriptors(commandList, m_inline_UAV, m_stale_UAV_bit_mask, [](ID3D12GraphicsCommandList* cmdList, u32 rootIndex, D3D12_GPU_VIRTUAL_ADDRESS address) { cmdList->SetComputeRootUnorderedAccessView(rootIndex, address); });
    }

    D3D12_GPU_DESCRIPTOR_HANDLE DynamicDescriptorHeap::copy_descriptor(CommandList& comandList, D3D12_CPU_DESCRIPTOR_HANDLE cpuDescriptor)
    {
      if(!m_current_descriptor_heap || m_num_free_handles < 1)
      {
        m_current_descriptor_heap       = request_descriptor_heap();
        m_current_CPU_descriptor_handle = m_current_descriptor_heap->GetCPUDescriptorHandleForHeapStart();
        m_current_GPU_descriptor_handle = m_current_descriptor_heap->GetGPUDescriptorHandleForHeapStart();
        m_num_free_handles              = m_num_descriptors_per_heap;

        comandList.set_descriptor_heap(m_descriptor_heap_type, m_current_descriptor_heap.Get());

        // When updating the descriptor heap on the command list, all descriptor
        // tables must be (re)recopied to the new descriptor heap (not just
        // the stale descriptor tables).
        m_stale_descriptor_table_bit_mask = m_descriptor_table_bit_mask;
      }

      auto d3d_device = m_device.d3d_device();

      D3D12_GPU_DESCRIPTOR_HANDLE h_GPU = m_current_GPU_descriptor_handle;
      d3d_device->CopyDescriptorsSimple(1, m_current_CPU_descriptor_handle, cpuDescriptor, m_descriptor_heap_type);

      m_current_CPU_descriptor_handle.Offset(1, m_descriptor_handle_increment_size);
      m_current_GPU_descriptor_handle.Offset(1, m_descriptor_handle_increment_size);
      m_num_free_handles -= 1;

      return h_GPU;
    }

    void DynamicDescriptorHeap::parse_root_signature(const rsl::shared_ptr<RootSignature>& rootSignature)
    {
      // If the root signature changes, all descriptors must be (re)bound to the
      // command list.
      m_stale_descriptor_table_bit_mask = 0;

      const auto& root_signature_desc = rootSignature->d3d_root_signature_description();

      // Get a bit mask that represents the root parameter indices that match the
      // descriptor heap type for this dynamic descriptor heap.
      m_descriptor_table_bit_mask   = rootSignature->descriptor_table_bit_mask(m_descriptor_heap_type);
      u32 descriptor_table_bit_mask = m_descriptor_table_bit_mask;

      u32 current_offset = 0;
      DWORD root_index;
      while(_BitScanForward(&root_index, descriptor_table_bit_mask) && root_index < root_signature_desc.NumParameters)
      {
        u32 num_descriptors = rootSignature->num_descriptors(root_index);

        DescriptorTableCache& descriptor_table_cache   = m_descriptor_table_cache[root_index];
        descriptor_table_cache.num_descriptors         = num_descriptors;
        descriptor_table_cache.base_descriptor         = m_descriptor_handle_cache.get() + current_offset;

        current_offset += num_descriptors;

        // Flip the descriptor table bit so it's not scanned again for the current index.
        descriptor_table_bit_mask ^= (1 << root_index);
      }

      // Make sure the maximum number of descriptors per descriptor heap has not been exceeded.
      REX_ASSERT_X(current_offset <= m_num_descriptors_per_heap, "The root signature requires more than the maximum number of descriptors per descriptor heap. Consider increasing the maximum number of descriptors per descriptor heap.");
    }

    void DynamicDescriptorHeap::reset()
    {
      m_available_descriptor_heaps = m_descriptor_heap_pool;

      m_current_descriptor_heap.Reset();
      m_current_CPU_descriptor_handle   = CD3DX12_CPU_DESCRIPTOR_HANDLE(D3D12_DEFAULT);
      m_current_GPU_descriptor_handle   = CD3DX12_GPU_DESCRIPTOR_HANDLE(D3D12_DEFAULT);
      m_num_free_handles                = 0;
      m_descriptor_table_bit_mask       = 0;
      m_stale_descriptor_table_bit_mask = 0;
      m_stale_CBV_bit_mask              = 0;
      m_stale_SRV_bit_mask              = 0;
      m_stale_UAV_bit_mask              = 0;

      // Reset the descriptor cache
      for(int i = 0; i < s_max_descriptor_tables; ++i)
      {
        m_descriptor_table_cache[i].reset();

        m_inline_CBV[i] = 0ull;
        m_inline_SRV[i] = 0ull;
        m_inline_UAV[i] = 0ull;
      }
    }

    wrl::ComPtr<ID3D12DescriptorHeap> DynamicDescriptorHeap::request_descriptor_heap()
    {
      wrl::ComPtr<ID3D12DescriptorHeap> descriptor_heap;
      if(!m_available_descriptor_heaps.empty())
      {
        descriptor_heap = m_available_descriptor_heaps.front();
        m_available_descriptor_heaps.pop();
      }
      else
      {
        descriptor_heap = create_descriptor_heap();
        m_descriptor_heap_pool.push(descriptor_heap);
      }

      return descriptor_heap;
    }

    wrl::ComPtr<ID3D12DescriptorHeap> DynamicDescriptorHeap::create_descriptor_heap()
    {
      auto d3d_device = m_device.d3d_device();

      D3D12_DESCRIPTOR_HEAP_DESC descriptor_heap_desc = {};
      descriptor_heap_desc.Type                       = m_descriptor_heap_type;
      descriptor_heap_desc.NumDescriptors             = m_num_descriptors_per_heap;
      descriptor_heap_desc.Flags                      = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

      wrl::ComPtr<ID3D12DescriptorHeap> descriptor_heap;
      if(DX_FAILED(d3d_device->CreateDescriptorHeap(&descriptor_heap_desc, IID_PPV_ARGS(&descriptor_heap))))
      {
        REX_ERROR(LogDirectX, "Failed to CreateDescriptorHeap");
        return nullptr;
      }

      return descriptor_heap;
    }

    u32 DynamicDescriptorHeap::compute_stale_descriptor_count() const
    {
      u32 num_stale_descriptors = 0;
      DWORD i;
      DWORD stale_descriptors_bit_mask = m_stale_descriptor_table_bit_mask;

      while(_BitScanForward(&i, stale_descriptors_bit_mask))
      {
        num_stale_descriptors += m_descriptor_table_cache[i].num_descriptors;
        stale_descriptors_bit_mask ^= (1 << i);
      }

      return num_stale_descriptors;
    }

    void DynamicDescriptorHeap::commit_descriptor_tables(CommandList& commandList, rsl::function<void(ID3D12GraphicsCommandList*, u32, D3D12_GPU_DESCRIPTOR_HANDLE)> setFunc)
    {
      // Compute the number of descriptors that need to be copied
      u32 num_descriptors_to_commit = compute_stale_descriptor_count();

      if(num_descriptors_to_commit > 0)
      {
        auto d3d_device                = m_device.d3d_device();
        auto d3d_graphics_command_list = commandList.graphics_command_list().Get();

        REX_ASSERT_X(d3d_graphics_command_list != nullptr, "Invalid command list");

        if(!m_current_descriptor_heap || m_num_free_handles < num_descriptors_to_commit)
        {
          m_current_descriptor_heap       = request_descriptor_heap();
          m_current_CPU_descriptor_handle = m_current_descriptor_heap->GetCPUDescriptorHandleForHeapStart();
          m_current_GPU_descriptor_handle = m_current_descriptor_heap->GetGPUDescriptorHandleForHeapStart();
          m_num_free_handles              = m_num_descriptors_per_heap;

          commandList.set_descriptor_heap(m_descriptor_heap_type, m_current_descriptor_heap.Get());

          // When updating the descriptor heap on the command list, all descriptor
          // tables must be (re)recopied to the new descriptor heap (not just
          // the stale descriptor tables).
          m_stale_descriptor_table_bit_mask = m_descriptor_table_bit_mask;
        }

        // Scan from LSB to MSB for a bit set in staleDescriptorsBitMask
        DWORD root_index;
        while(_BitScanForward(&root_index, m_stale_descriptor_table_bit_mask))
        {
          UINT num_src_descriptors                         = m_descriptor_table_cache[root_index].num_descriptors;
          D3D12_CPU_DESCRIPTOR_HANDLE* sc_dcriptor_handles = m_descriptor_table_cache[root_index].base_descriptor;

          D3D12_CPU_DESCRIPTOR_HANDLE dest_descriptor_range_starts[] = {m_current_CPU_descriptor_handle};
          UINT dest_descriptor_range_sizes[]                         = {num_src_descriptors};

          // Copy the staged CPU visible descriptors to the GPU visible descriptor heap.
          d3d_device->CopyDescriptors(1, dest_descriptor_range_starts, dest_descriptor_range_sizes, num_src_descriptors, sc_dcriptor_handles, nullptr, m_descriptor_heap_type);

          // Set the descriptors on the command list using the passed-in setter function.
          setFunc(d3d_graphics_command_list, root_index, m_current_GPU_descriptor_handle);

          // Offset current CPU and GPU descriptor handles.
          m_current_CPU_descriptor_handle.Offset(num_src_descriptors, m_descriptor_handle_increment_size);
          m_current_GPU_descriptor_handle.Offset(num_src_descriptors, m_descriptor_handle_increment_size);
          m_num_free_handles -= num_src_descriptors;

          // Flip the stale bit so the descriptor table is not recopied again unless it is updated with a new
          // descriptor.
          m_stale_descriptor_table_bit_mask ^= (1 << root_index);
        }
      }
    }

    void DynamicDescriptorHeap::commit_inline_descriptors(CommandList& commandList, const D3D12_GPU_VIRTUAL_ADDRESS* bufferLocations, u32& bitMask, rsl::function<void(ID3D12GraphicsCommandList*, u32, D3D12_GPU_VIRTUAL_ADDRESS)> setFunc)
    {
      if(bitMask != 0)
      {
        auto d3d_graphics_command_list = commandList.graphics_command_list().Get();

        DWORD root_index;
        while(_BitScanForward(&root_index, bitMask))
        {
          setFunc(d3d_graphics_command_list, root_index, bufferLocations[root_index]);

          // Flip the stale bit so the descriptor is not recopied again unless it is updated with a new descriptor.
          bitMask ^= (1 << root_index);
        }
      }
    }
  } // namespace renderer
} // namespace rex