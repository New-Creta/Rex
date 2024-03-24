#pragma once

#include "rex_directx/utility/directx_util.h"

#include "rex_renderer_core/resource_management/resource.h"

namespace rex
{
  namespace rhi
  {
    class Resource;

    struct UploadInfo
    {
      Resource* dst_resource;
      void* data;
      s32 size;
    };

    class CommandList;
    class UploadBuffer : public BaseResource<ID3D12Resource>
    {
    public:
      UploadBuffer(ResourceHash hash, const wrl::ComPtr<ID3D12Resource>& uploadBuffer, D3D12_RESOURCE_STATES startState = D3D12_RESOURCE_STATE_COMMON);
      ~UploadBuffer();

      void write(CommandList* cmdList, Resource* dstResource, const void* data, s32 size);

      void upload(CommandList* cmdList);

    private:
      wrl::ComPtr<ID3D12Resource> m_upload_buffer;
      void* m_mapped_data;
      s32 m_offset;
      D3D12_RESOURCE_STATES m_resource_state;
      rsl::vector<UploadInfo> m_upload_infos;
    };
  }
}