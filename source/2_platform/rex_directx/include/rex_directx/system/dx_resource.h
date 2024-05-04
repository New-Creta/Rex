#pragma once

#include "rex_directx/utility/dx_util.h"
#include "rex_engine/engine/types.h"
#include "rex_renderer_core/resource_management/resource.h"

namespace rex
{
  namespace rhi
  {
    class UploadBuffer;

    class Resource : public BaseResource<ID3D12Resource>
    {
    public:
      Resource(const wrl::ComPtr<ID3D12Resource>& resource, D3D12_RESOURCE_STATES startState, s64 size, DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN);
      Resource(const wrl::ComPtr<ID3D12Resource>& resource, D3D12_RESOURCE_STATES startState, s32 width, s32 height, DXGI_FORMAT format);

      s32 width() const;
      s32 height() const;
      s64 size() const;
      DXGI_FORMAT format() const;
      ID3D12Resource* get() const;
      D3D12_RESOURCE_STATES resource_state() const;

      void transition(ID3D12GraphicsCommandList* cmdList, D3D12_RESOURCE_STATES to);
      void write_buffer(ID3D12GraphicsCommandList* cmdList, UploadBuffer* uploadBuffer, s64 start, s64 size, s32 offset);
      void write_texture(ID3D12GraphicsCommandList* cmdList, UploadBuffer* uploadBuffer, s64 width, s64 height, s64 rowPitch);
      void copy_to(ID3D12GraphicsCommandList* cmdList, ID3D12Resource* srcResource);

    private:
      wrl::ComPtr<ID3D12Resource> m_resource;
      DXGI_FORMAT m_format;
      s32 m_width;
      s32 m_height;
      s64 m_size;
      D3D12_RESOURCE_STATES m_resource_state;
    };
  } // namespace rhi
} // namespace rex