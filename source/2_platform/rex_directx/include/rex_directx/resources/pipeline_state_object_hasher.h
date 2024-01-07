#pragma once

#include "rex_std/bonus/functional/hash_result.h"

namespace rex
{
  namespace renderer
  {
    class InputLayoutResource;
    class ShaderProgramResource;
    class RasterStateResource;
    //class BlendStateResource;
    //class DepthStencilStateResource;

    struct PipelineStateObjectHashData
    {
      const InputLayoutResource* input_layout_resource;
      const ShaderProgramResource* shader_program_resource;
      const RasterStateResource* raster_state_resource;
      //BlendStateResource* blend_state_resource;
      //DepthStencilStateResource* depth_stencil_state_resource;
    };

    rsl::hash_result create_pso_hash(const PipelineStateObjectHashData& desc);
  } // namespace renderer
} // namespace rex