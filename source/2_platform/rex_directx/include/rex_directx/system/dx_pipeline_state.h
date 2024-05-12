#pragma once

#include "rex_directx/utility/dx_util.h"
#include "rex_renderer_core/resource_management/resource.h"

#include "rex_renderer_core/resources/blend_state.h"
#include "rex_renderer_core/resources/depth_stencil_state.h"

#include "rex_directx/resources/dx_input_layout_resource.h"
#include "rex_directx/system/dx_vertex_shader.h"
#include "rex_directx/system/dx_pixel_shader.h"
#include "rex_directx/resources/dx_raster_state_resource.h"

namespace rex
{
  namespace rhi
  {
    struct PipelineStateDesc
    {
      InputLayoutResource* input_layout;
      RootSignature* root_signature;
      VertexShader* vertex_shader;
      PixelShader* pixel_shader;
      RasterStateResource* raster_state;
      rsl::optional<BlendDesc> blend_state;
      rsl::optional<DepthStencilDesc> depth_stencil_state;
    };

    class PipelineState : public BaseResource<ID3D12PipelineState>
    {
    public:
      PipelineState(const wrl::ComPtr<ID3D12PipelineState>& pso, ResourceID id);

    private:
      wrl::ComPtr<ID3D12PipelineState> m_pso;
    };
  } // namespace rhi
} // namespace rex