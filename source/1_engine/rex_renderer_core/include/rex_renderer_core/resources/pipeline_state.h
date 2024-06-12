#pragma once

#include "rex_std/optional.h"

#include "rex_renderer_core/resources/raster_state.h"
#include "rex_renderer_core/resources/blend_state.h"
#include "rex_renderer_core/resources/depth_stencil_state.h"

namespace rex
{
  namespace rhi
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
      rsl::optional<RasterStateResource> raster_state;
      rsl::optional<BlendDesc> blend_state;
      rsl::optional<DepthStencilDesc> depth_stencil_state;
    };

    // Base class for the pipeline state, just acts as an interface
    class PipelineState
    {
      // Nothing to implement
    };
  } // namespace rhi
} // namespace rex

namespace rsl
{

} // namespace rsl