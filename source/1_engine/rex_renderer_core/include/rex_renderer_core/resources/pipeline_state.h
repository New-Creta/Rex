#pragma once

#include "rex_std/optional.h"

#include "rex_renderer_core/resources/raster_state.h"
#include "rex_renderer_core/resources/blend_state.h"
#include "rex_renderer_core/resources/depth_stencil_state.h"
#include "rex_renderer_core/resources/input_layout.h"
#include "rex_renderer_core/system/shader_pipeline.h"

#include "rex_renderer_core/gfx/primitive_topology.h"

#include "rex_std/vector.h"

namespace rex
{
  namespace gfx
  {
    class InputLayout;
    class RootSignature;
    class Shader;

    // A descriptor describing the pipeline state to be constructed
    struct PipelineStateDesc
    {
      InputLayoutDesc input_layout;
      ShaderPipeline shader_pipeline;
      PrimitiveTopologyType primitive_topology = PrimitiveTopologyType::Triangle;
      RasterStateDesc raster_state;
      BlendDesc blend_state;
      DepthStencilDesc depth_stencil_state;
    };

    // Base class for the pipeline state, just acts as an interface
    class PipelineState
    {
    public:
      PipelineState(RootSignature* rootSignatre)
        : m_root_signature(rootSignatre)
      {}
      virtual ~PipelineState() = default;

      RootSignature* root_signature()
      {
        return m_root_signature;
      }

    private:
      RootSignature* m_root_signature;
    };
  } // namespace gfx
} // namespace rex

namespace rsl
{

} // namespace rsl