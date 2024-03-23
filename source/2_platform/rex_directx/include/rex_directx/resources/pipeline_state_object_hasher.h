#pragma once

#include "rex_std/bonus/functional/hash_result.h"

#include "rex_engine/engine/types.h"
#include "rex_renderer_core/commands/create_constant_layout_description_cmd.h"

#include "rex_renderer_core/resources/pipeline_state.h"

namespace rex
{
  namespace rhi
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
    bool operator==(const rex::renderer::PipelineStateObjectHashData& lhs, const rex::renderer::PipelineStateObjectHashData& rhs);
    bool operator!=(const rex::renderer::PipelineStateObjectHashData& lhs, const rex::renderer::PipelineStateObjectHashData& rhs);

    namespace internal
    {
      rsl::hash_result hash_input_layout_resource(const InputLayoutResource* inputLayoutResource);
      rsl::hash_result hash_shader_program_constants(const commands::ConstantLayoutDescription* constants, s32 numConstants);
      rsl::hash_result hash_shader_program_resource(const ShaderProgramResource* shaderProgramResource);
      rsl::hash_result hash_raster_state_resource(const RasterStateResource* rasterStateResource);
    }

  } // namespace renderer
} // namespace rex

