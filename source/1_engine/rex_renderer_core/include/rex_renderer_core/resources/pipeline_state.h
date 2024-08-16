#pragma once

#include "rex_std/optional.h"

#include "rex_renderer_core/resources/raster_state.h"
#include "rex_renderer_core/resources/blend_state.h"
#include "rex_renderer_core/resources/depth_stencil_state.h"
#include "rex_renderer_core/resources/input_layout.h"
#include "rex_renderer_core/system/shader_pipeline.h"
#include "rex_renderer_core/system/output_merger.h"

#include "rex_renderer_core/gfx/primitive_topology.h"
#include "rex_renderer_core/gfx/texture_format.h"

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
      InputLayoutDesc input_layout;                                               // The input layout that'll be used by the pipeline
      ShaderPipeline shader_pipeline;                                             // The shader pipeline holds all the shaders that'll be used by the PSO
      PrimitiveTopologyType primitive_topology = PrimitiveTopologyType::Triangle; // This is used for geometry, hull or domain shaders
      OutputMergerDesc output_merger;                                             // The output merger configuration that'll be used by the pipeline
      TextureFormat dsv_format = TextureFormat::Unknown;                          // The depth stencil format to be used for the graphics pipeline
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
      // The root signature holds which parameters are used by the shader
      // The gfx API pipeline state uses it to verify itself on construction, but doesn't store it internally
      // So we have to store it ourselves and manually set it when the pso gets bound
      RootSignature* m_root_signature;
    };
  } // namespace gfx
} // namespace rex
