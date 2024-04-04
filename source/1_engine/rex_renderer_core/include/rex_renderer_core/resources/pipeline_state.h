#pragma once

#include "rex_std/functional.h"

#include "rex_renderer_core/resource_management/resource_slot.h"

namespace rex
{
  namespace rhi
  {
    struct PipelineStateDesc
    {
      ResourceSlot input_layout;
      ResourceSlot shader;
      ResourceSlot raster_state;
      ResourceSlot blend_state;
      ResourceSlot depth_stencil_state;
    };
  }
}

namespace rsl
{
  template <>
  struct hash<rex::rhi::PipelineStateDesc>
  {
    rsl::hash_result operator()(const rex::rhi::PipelineStateDesc& psoDesc) const
    {
      const char* buffer = reinterpret_cast<const char*>(&psoDesc);
      const size_t buffer_size = sizeof(psoDesc);

      return rsl::internal::hash(buffer, buffer_size);
    }
  };
}