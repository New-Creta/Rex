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

    namespace internal
    {
      rsl::hash_result hash_input_layout_resource(const InputLayoutResource* inputLayoutResource);
      rsl::hash_result hash_shader_program_constants(const commands::ConstantLayoutDescription* constants, s32 numConstants);
      rsl::hash_result hash_shader_program_resource(const ShaderProgramResource* shaderProgramResource);
      rsl::hash_result hash_raster_state_resource(const RasterStateResource* rasterStateResource);
    }

  } // namespace renderer
} // namespace rex

namespace rsl
{
  template <>
  struct hash<rex::renderer::PipelineStateObjectHashData>
  {
    rsl::hash_result operator()(const rex::renderer::PipelineStateObjectHashData& hashData)
    {
      rsl::hash_result seed = 0;

      seed = rsl::internal::hash_combine(seed, rex::renderer::internal::hash_input_layout_resource(hashData.input_layout_resource));
      seed = rsl::internal::hash_combine(seed, rex::renderer::internal::hash_shader_program_resource(hashData.shader_program_resource));
      seed = rsl::internal::hash_combine(seed, rex::renderer::internal::hash_raster_state_resource(hashData.raster_state_resource));

      return seed;
    }
  };
}