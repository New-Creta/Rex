#pragma once

#include "rex_directx/utility/dx_util.h"

#include "rex_renderer_core/resource_management/resource.h"

namespace rex
{
  namespace rhi
  {
    class Resource;

    class CommandList;
    class UploadBuffer : public BaseResource<ID3D12Resource>
    {
    public:
      UploadBuffer(const wrl::ComPtr<ID3D12Resource>& uploadBuffer, D3D12_RESOURCE_STATES startState = D3D12_RESOURCE_STATE_COMMON);
      ~UploadBuffer();

      void write(CommandList* cmdList, Resource* dstResource, const void* data, s64 size);

      void reset();

    private:
      wrl::ComPtr<ID3D12Resource> m_upload_buffer;
      void* m_mapped_data;
      s64 m_offset;
      D3D12_RESOURCE_STATES m_resource_state;
    };
  }
}