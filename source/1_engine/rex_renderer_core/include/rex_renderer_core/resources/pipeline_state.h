#pragma once

#include "rex_std/optional.h"

#include "rex_renderer_core/resources/raster_state.h"
#include "rex_renderer_core/resources/blend_state.h"
#include "rex_renderer_core/resources/depth_stencil_state.h"

#include "rex_renderer_core/gfx/primitive_topology.h"

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
      InputLayout* input_layout;
      RootSignature* root_signature;
      Shader* vertex_shader;
      Shader* pixel_shader;
      PrimitiveTopologyType primitive_topology = PrimitiveTopologyType::Triangle;
      rsl::optional<RasterState> raster_state;
      rsl::optional<BlendDesc> blend_state;
      rsl::optional<DepthStencilDesc> depth_stencil_state;
    };

    // Base class for the pipeline state, just acts as an interface
    class PipelineState
    {
    public:
      virtual ~PipelineState() = default;
    };
  } // namespace gfx
} // namespace rex

namespace rsl
{

} // namespace rsl