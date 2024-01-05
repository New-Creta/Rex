#include "rex_directx/resources/pipeline_state_object_hasher.h"

#include "rex_directx/resources/input_layout_resource.h"
#include "rex_directx/resources/raster_state_resource.h"
#include "rex_directx/resources/shader_program_resource.h"

#include "rex_std/internal/functional/hash.h"

namespace rex
{
  namespace renderer
  {
    namespace internal
    {
      //-------------------------------------------------------------------------
      rsl::hash_result hash_input_layout_resource(const InputLayoutResource* inputLayoutResource) 
      {
        const rsl::vector<D3D12_INPUT_ELEMENT_DESC>* input_layout_descs = inputLayoutResource->get();

        rsl::hash_result seed = rsl::hash<rsl::count_t> {}(input_layout_descs->size());

        for (const D3D12_INPUT_ELEMENT_DESC& desc : *input_layout_descs)
        {
          seed = rsl::internal::hash_combine(seed, rsl::hash<UINT> {}(desc.AlignedByteOffset));    
          seed = rsl::internal::hash_combine(seed, rsl::hash<DXGI_FORMAT> {}(desc.Format));    
          seed = rsl::internal::hash_combine(seed, rsl::hash<UINT> {} (desc.InputSlot));    
          seed = rsl::internal::hash_combine(seed, rsl::hash<D3D12_INPUT_CLASSIFICATION> {} (desc.InputSlotClass));    
          seed = rsl::internal::hash_combine(seed, rsl::hash<UINT> {} (desc.InstanceDataStepRate));    
          seed = rsl::internal::hash_combine(seed, rsl::hash<UINT> {} (desc.SemanticIndex));    
          seed = rsl::internal::hash_combine(seed, rsl::hash<LPCSTR> {} (desc.SemanticName));    
        }

        return seed;
      }

      //-------------------------------------------------------------------------
      rsl::hash_result hash_shader_program_constants(const commands::ConstantLayoutDescription* constants, s32 numConstants)
      {
        rsl::hash_result seed = rsl::hash<s32> {}(numConstants);

        for(s32 i = 0; i < numConstants; ++i)
        {
          seed = rsl::internal::hash_combine(seed, rsl::hash<s32> {}(constants[i].location));
          seed = rsl::internal::hash_combine(seed, rsl::hash<commands::ConstantType> {}(constants[i].type));
          seed = rsl::internal::hash_combine(seed, rsl::hash<rsl::string_view> {}(constants[i].name));
        }

        return seed;
      }

      //-------------------------------------------------------------------------
      rsl::hash_result hash_shader_program_resource(const ShaderProgramResource* shaderProgramResource)
      {
        const resources::ShaderProgram* shader_program = shaderProgramResource->get();

        rsl::hash_result seed = hash_shader_program_constants(shader_program->constants, shader_program->num_constants);

        seed = rsl::internal::hash_combine(seed, (rsl::intptr) shader_program->vertex_shader.Get());
        seed = rsl::internal::hash_combine(seed, (rsl::intptr) shader_program->pixel_shader.Get());
        seed = rsl::internal::hash_combine(seed, (rsl::intptr) shader_program->root_signature.Get());

        return seed;
      }

      //-------------------------------------------------------------------------
      rsl::hash_result hash_raster_state_resource(const RasterStateResource* rasterStateResource)
      {
        const D3D12_RASTERIZER_DESC* raster_state = rasterStateResource->get();

        rsl::hash_result seed = rsl::hash<BOOL> {}(raster_state->AntialiasedLineEnable);

        seed = rsl::internal::hash_combine(seed, rsl::hash<D3D12_CONSERVATIVE_RASTERIZATION_MODE> {}(raster_state->ConservativeRaster));
        seed = rsl::internal::hash_combine(seed, rsl::hash<D3D12_CULL_MODE> {}(raster_state->CullMode));
        seed = rsl::internal::hash_combine(seed, rsl::hash<INT>{} (raster_state->DepthBias));
        seed = rsl::internal::hash_combine(seed, rsl::hash<FLOAT>{} (raster_state->DepthBiasClamp));
        seed = rsl::internal::hash_combine(seed, rsl::hash<BOOL>{} (raster_state->DepthClipEnable));
        seed = rsl::internal::hash_combine(seed, rsl::hash<D3D12_FILL_MODE> {}(raster_state->FillMode));
        seed = rsl::internal::hash_combine(seed, rsl::hash<UINT>{} (raster_state->ForcedSampleCount));
        seed = rsl::internal::hash_combine(seed, rsl::hash<BOOL>{} (raster_state->FrontCounterClockwise));
        seed = rsl::internal::hash_combine(seed, rsl::hash<BOOL>{} (raster_state->MultisampleEnable));
        seed = rsl::internal::hash_combine(seed, rsl::hash<FLOAT>{} (raster_state->SlopeScaledDepthBias));

        return seed;
      }
    } // namespace internal

    //-------------------------------------------------------------------------
    rsl::hash_result create_pso_hash(const PipelineStateObjectHashData& desc) 
    {
      rsl::hash_result seed = 0;

      rsl::internal::hash_combine(seed, internal::hash_input_layout_resource(desc.input_layout_resource));
      rsl::internal::hash_combine(seed, internal::hash_shader_program_resource(desc.shader_program_resource));
      rsl::internal::hash_combine(seed, internal::hash_raster_state_resource(desc.raster_state_resource));

      return seed;
    }
  } // namespace renderer
} // namespace rex