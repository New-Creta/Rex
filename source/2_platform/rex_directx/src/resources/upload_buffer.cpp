#include "rex_directx/resources/upload_buffer.h"

#include "rex_directx/d3dx12.h"

#include "rex_directx/diagnostics/directx_call.h"

#include "rex_directx/system/directx_commandlist.h"
#include "rex_directx/system/directx_resource.h"

namespace rex
{
  namespace rhi
  {
    UploadBuffer::UploadBuffer(const wrl::ComPtr<ID3D12Resource>& uploadBuffer, D3D12_RESOURCE_STATES startState)
      : BaseResource(uploadBuffer.Get(), make_new_hash())
      , m_upload_buffer(uploadBuffer)
      , m_mapped_data(nullptr)
      , m_resource_state(startState)
      , m_offset(0)
    {
      CD3DX12_RANGE read_range(0, 0); // We do not intend to read from this resource on the CPU
      DX_CALL(m_upload_buffer->Map(0, &read_range, &m_mapped_data));
    }

    UploadBuffer::~UploadBuffer()
    {
      m_upload_buffer->Unmap(0, nullptr);
    }
 
    void UploadBuffer::write(CommandList* cmdList, Resource* dstResource, const void* data, s32 size)
    {
      // Write the data into our mapped memory
      rsl::byte* start = (rsl::byte*)m_mapped_data + m_offset;
      rsl::memcpy(start, data, size);

      // Fill the commandlist with the commands to update this resource
      D3D12_RESOURCE_STATES original_state = dstResource->resource_state();
      dstResource->transition(cmdList->get(), D3D12_RESOURCE_STATE_COPY_DEST);
      dstResource->write(cmdList->get(), this, m_offset, size);
      dstResource->transition(cmdList->get(), original_state);
      
      //m_upload_infos.emplace_back(UploadInfo{ dstResource, m_offset, size });
      m_offset += size;
    }

    void UploadBuffer::reset()
    {
      m_offset = 0;
    }

  }
}