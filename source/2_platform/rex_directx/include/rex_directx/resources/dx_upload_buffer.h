#pragma once

#include "rex_directx/utility/dx_util.h"
#include "rex_renderer_core/resource_management/resource.h"

namespace rex
{
  namespace rhi
  {
    class Resource;

    class UploadBuffer : public BaseResource<ID3D12Resource>
    {
    public:
      explicit UploadBuffer(const wrl::ComPtr<ID3D12Resource>& uploadBuffer, D3D12_RESOURCE_STATES startState = D3D12_RESOURCE_STATE_COMMON);
      UploadBuffer(const UploadBuffer&) = delete;
      UploadBuffer(UploadBuffer&&) = delete;
      ~UploadBuffer() override;

      UploadBuffer& operator=(const UploadBuffer&) = delete;
      UploadBuffer& operator=(UploadBuffer&&) = delete;

      void write(ID3D12GraphicsCommandList* cmdList, Resource* dstResource, const void* data, s64 size, s32 alignment = 1, s32 offset = 0);
      void write_texture(ID3D12GraphicsCommandList* cmdList, Resource* dstResource, const void* data, DXGI_FORMAT format, s64 width, s64 height);

      void reset();

    private:
      wrl::ComPtr<ID3D12Resource> m_upload_buffer;
      void* m_mapped_data;
      s64 m_offset;
      s64 m_size;
      D3D12_RESOURCE_STATES m_resource_state;
    };
  } // namespace rhi
} // namespace rex