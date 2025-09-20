#pragma once

#include "rex_std/optional.h"

#include "rex_engine/gfx/resources/resource.h"

#include "rex_engine/gfx/resources/raster_state.h"
#include "rex_engine/gfx/resources/blend_state.h"
#include "rex_engine/gfx/resources/depth_stencil_state.h"
#include "rex_engine/gfx/resources/input_layout.h"
#include "rex_engine/gfx/system/shader_pipeline.h"
#include "rex_engine/gfx/system/output_merger.h"

#include "rex_engine/gfx/core/primitive_topology.h"
#include "rex_engine/gfx/core/texture_format.h"

namespace rex
{
  namespace gfx
  {
    class InputLayout;
    class RootSignature;
    class Shader;

    // A pipeline state is all configuration for the render pipeline
    // It holds the following information
    // - all shaders
    // - all shader parameter types (not values)
    // - input layout
    // - primitive topology
    // - rasterizer state
    // - blend state
    // - depth stencil state
    // - render target formats
    // - depth stencil formats
    //
    // A pipeline state is created using a descriptor holding the above values and immediately serialized for most optimal storage
    // Duplicate pipeline states get deduplicated at runtime and cached in a pipeline state library
    // The pipeline state library gets loaded at init time as pipeline state creation is quite expensive

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
    class PipelineState : public Resource
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
