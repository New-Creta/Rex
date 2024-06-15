#pragma once

#include "rex_directx/utility/dx_util.h"
#include "rex_renderer_core/resource_management/resource.h"

#include "rex_renderer_core/resources/blend_state.h"
#include "rex_renderer_core/resources/depth_stencil_state.h"

#include "rex_directx/resources/dx_input_layout.h"
#include "rex_directx/resources/dx_vertex_shader.h"
#include "rex_directx/resources/dx_pixel_shader.h"
#include "rex_renderer_core/resources/pipeline_state.h"

namespace rex
{
  namespace rhi
  {
    class InputLayout;
    class RootSignature;
    class RasterState;

    class DxPipelineState : public PipelineState
    {
    public:
      DxPipelineState(const wrl::ComPtr<ID3D12PipelineState>& pso);

      ID3D12PipelineState* get();

    private:
      wrl::ComPtr<ID3D12PipelineState> m_pso;
    };
  } // namespace rhi
} // namespace rex