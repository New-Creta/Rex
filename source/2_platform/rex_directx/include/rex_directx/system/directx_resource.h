#pragma once

#include "rex_directx/utility/directx_util.h"

#include "rex_renderer_core/resource_management/resource.h"

#include "rex_engine/engine/types.h"

namespace rex
{
  namespace rhi
  {
    class UploadBuffer;

    class Resource : public BaseResource<ID3D12Resource>
    {
    public:
      Resource(ResourceHash hash, const wrl::ComPtr<ID3D12Resource>& resource, D3D12_RESOURCE_STATES startState);

      s32 width() const;
      s32 height() const;
      DXGI_FORMAT format() const;
      ID3D12Resource* get() const;
      D3D12_RESOURCE_STATES resource_state() const;

      void transition(ID3D12GraphicsCommandList* cmdList, D3D12_RESOURCE_STATES to);
      void write(ID3D12GraphicsCommandList* cmdList, UploadBuffer* uploadBuffer, const void* data, s32 size);
      void copy_to(ID3D12GraphicsCommandList* cmdList, ID3D12Resource* srcResource);

    private:
      wrl::ComPtr<ID3D12Resource> m_resource;
      DXGI_FORMAT m_format;
      s32 m_width;
      s32 m_height;
      D3D12_RESOURCE_STATES m_resource_state;
    };
  }
}