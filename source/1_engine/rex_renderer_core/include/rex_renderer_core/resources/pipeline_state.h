#pragma once

#include "rex_renderer_core/resource_management/resource_slot.h"
#include "rex_std/functional.h"
#include "rex_std/optional.h"

#include "rex_renderer_core/resources/blend_state.h"
#include "rex_renderer_core/resources/depth_stencil_state.h"

namespace rex
{
  namespace rhi
  {
    struct PipelineStateDesc
    {
      ResourceSlot input_layout;
      ResourceSlot shader;
      ResourceSlot raster_state;
      rsl::optional<BlendState> blend_state;
      rsl::optional<DepthStencilDesc> depth_stencil_state;
    };
  } // namespace rhi
} // namespace rex

namespace rsl
{
  template <>
  struct hash<rex::rhi::PipelineStateDesc>
  {
    rsl::hash_result operator()(const rex::rhi::PipelineStateDesc& psoDesc) const
    {
      const char* buffer       = reinterpret_cast<const char*>(&psoDesc); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
      const size_t buffer_size = sizeof(psoDesc);

      return rsl::internal::hash(buffer, buffer_size);
    }
  };
} // namespace rsl