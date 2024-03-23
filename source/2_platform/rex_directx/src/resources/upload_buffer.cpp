#include "rex_directx/resources/upload_buffer.h"

#include "rex_directx/d3dx12.h"

#include "rex_directx/diagnostics/directx_call.h"

#include "rex_directx/system/directx_commandlist.h"

namespace rex
{
  namespace rhi
  {
    UploadBuffer::UploadBuffer(const wrl::ComPtr<ID3D12Resource>& uploadBuffer, D3D12_RESOURCE_STATES startState)
      : BaseResource()
      , m_upload_buffer(uploadBuffer)
      , m_mapped_data(nullptr)
      , m_resource_state(startState)
    {
      CD3DX12_RANGE read_range(0, 0); // We do not intend to read from this resource on the CPU
      DX_CALL(m_upload_buffer->Map(0, &read_range, &m_mapped_data));
    }

    UploadBuffer::~UploadBuffer()
    {
      m_upload_buffer->Unmap(0, nullptr);
    }
 
    void UploadBuffer::write(Resource* dstResource, void* data, s32 size)
    {
      rsl::byte* start = (rsl::byte*)m_mapped_data + m_offset;
      rsl::memcpy(start, data, size);

      m_upload_infos.emplace_back(UploadInfo{ dstResource, start, size });
      m_offset += size;
    }

    void UploadBuffer::upload(CommandList* cmdList)
    {
      for (const UploadInfo& upload_info : m_upload_infos)
      {
        D3D12_RESOURCE_STATES original_state = upload_info.dst_resource->resource_state();
        upload_info.dst_resource->transition(cmdList, D3D12_RESOURCE_STATE_COPY_DEST);
        upload_info.dst_resource->write(cmdList, this, upload_info.data, upload_info.size);
        upload_info.dst_resource->transition(cmdList, original_state);
      }

      m_upload_infos.clear();
    }

  }
}