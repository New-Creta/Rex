#pragma once

#include "rex_renderer_core/resource_management/resource_slot.h"
#include "rex_std/functional.h"
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

    class PipelineState
    {

    };
  } // namespace rhi
} // namespace rex

namespace rsl
{
  //template <>
  //struct hash<rex::rhi::PipelineStateDesc>
  //{
  //  rsl::hash_result operator()(const rex::rhi::PipelineStateDesc& psoDesc) const
  //  {
  //    const char* buffer       = reinterpret_cast<const char*>(&psoDesc); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
  //    const size_t buffer_size = sizeof(psoDesc);

  //    return rsl::internal::hash(buffer, buffer_size);
  //  }
  //};
} // namespace rsl