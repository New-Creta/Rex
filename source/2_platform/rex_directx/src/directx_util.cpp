#include "rex_directx/directx_util.h"
#include "rex_directx/d3dx12.h"
#include "rex_directx/log.h"

namespace rex
{
  namespace renderer
  {
    namespace directx
    {
      //-------------------------------------------------------------------------
      wrl::com_ptr<ID3D12Resource> create_default_buffer(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, const void* initData, UINT64 byteSize, wrl::com_ptr<ID3D12Resource>& uploadBuffer) 
      {
        wrl::com_ptr<ID3D12Resource> default_buffer;

        // Create the actual default buffer resource.
        if (FAILED(device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(byteSize), D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(default_buffer.GetAddressOf()))))
        {
          REX_ERROR(LogDirectX, "Failed to create commited resource for a default buffer.");
          return nullptr;
        }

        // In order to copy CPU memory data into our default buffer, we need to create
        // an intermediate upload heap.
        if (FAILED(device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(byteSize), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(uploadBuffer.GetAddressOf()))))
        {
          REX_ERROR(LogDirectX, "Failed to create commited resource for intermediate upload heap.");
          return nullptr;
        }

        // Describe the data we want to copy into the default buffer.
        D3D12_SUBRESOURCE_DATA sub_resource_data = {};
        sub_resource_data.pData = initData;
        sub_resource_data.RowPitch = byteSize;
        sub_resource_data.SlicePitch = sub_resource_data.RowPitch;

        // Schedule to copy the data to the default buffer resource.  
        // At a high level, the helper function UpdateSubresources will copy the CPU memory into the intermediate upload heap.
        // Then, using ID3D12CommandList::CopySubresourceRegion, the intermediate upload heap data will be copied to mBuffer.
        cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(default_buffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST));
        UpdateSubresources<1>(cmdList, default_buffer.Get(), uploadBuffer.Get(), 0, 0, 1, &sub_resource_data);
        cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(default_buffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));

        // Note: uploadbuffer has to be kept alive after the above function calls because
        // the command list has not been executed yet that performs the actual copy.
        // The caller can Release the uploadBuffer after it knows the copy has been executed.

        return default_buffer;
      }
    } // namespace directx
  }   // namespace renderer
} // namespace rex