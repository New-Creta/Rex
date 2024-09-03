#pragma once

#include "rex_directx/utility/dx_util.h"


#include "rex_engine/gfx/resources/blend_state.h"
#include "rex_engine/gfx/resources/depth_stencil_state.h"

#include "rex_directx/resources/dx_input_layout.h"
#include "rex_directx/resources/dx_vertex_shader.h"
#include "rex_directx/resources/dx_pixel_shader.h"
#include "rex_engine/gfx/resources/pipeline_state.h"

namespace rex
{
  namespace gfx
  {
    class InputLayout;
    class RootSignature;
    class RasterState;

    class DxPipelineState : public PipelineState
    {
    public:
      DxPipelineState(const wrl::ComPtr<ID3D12PipelineState>& pso, RootSignature* rootSignature);

      ID3D12PipelineState* dx_object();

    private:
      wrl::ComPtr<ID3D12PipelineState> m_pso;
    };
  } // namespace gfx
} // namespace rex