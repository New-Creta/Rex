#include "rex_directx/resources/shader_library.h"

#include "rex_directx/directx_call.h"
#include "rex_directx/directx_device.h"
#include "rex_directx/log.h"
#include "rex_directx/resources/root_signature.h"
#include "rex_directx/utility/vertex_types.h"
#include "rex_std/unordered_map.h"

#include <d3dcompiler.h>

namespace rex
{
  namespace renderer
  {
    namespace shader_library
    {
      namespace internal
      {
        rsl::shared_ptr<RootSignature> load_unlit_root_signature(Device* device)
        {
          // Create a root signature.
          // Allow input layout and deny unnecessary access to certain pipeline stages.
          D3D12_ROOT_SIGNATURE_FLAGS root_signature_flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
                                                            D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

          CD3DX12_ROOT_PARAMETER1 root_parameters[root_parameters::Unlit::NUM_ROOT_PARAMETERS];
          root_parameters[root_parameters::Unlit::MATRICES_CB].InitAsConstantBufferView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_VERTEX);

          CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC root_signature_description;
          root_signature_description.Init_1_1(root_parameters::Unlit::NUM_ROOT_PARAMETERS, root_parameters, 0, nullptr, root_signature_flags);

          return device->create_root_signature(root_signature_description.Desc_1_1);
        }

        wrl::ComPtr<ID3DBlob> compile_unlit_vertex_shader()
        {
          static const char* vertex_shader_code = "                                         \
                                                                                            \
          struct ModelViewProjection                                                        \
          {                                                                                 \
            matrix MVP;                                                                     \
          };                                                                                \
                                                                                            \
          ConstantBuffer<ModelViewProjection> ModelViewProjectionCB : register(b0);         \
                                                                                            \
          struct VertexPosColor                                                             \
          {                                                                                 \
            float3 Position : POSITION;                                                     \
            float3 Color : COLOR;                                                           \
          };                                                                                \
                                                                                            \
          struct VertexShaderOutput                                                         \
          {                                                                                 \
            float4 Color : COLOR;                                                           \
            float4 Position : SV_Position;                                                  \
          };                                                                                \
                                                                                            \
          VertexShaderOutput main(VertexPosColor IN)                                        \
          {                                                                                 \
            VertexShaderOutput OUT;                                                         \
                                                                                            \
            OUT.Position = mul(ModelViewProjectionCB.MVP, float4(IN.Position, 1.0f));       \
            OUT.Color    = float4(IN.Color, 1.0f);                                          \
                                                                                            \
            return OUT;                                                                     \
          }";

          static shader_library::ShaderCompilationDesc vertex_shader_compile_desc;
          vertex_shader_compile_desc.shader_code           = memory::make_blob(vertex_shader_code, (s32)strlen(vertex_shader_code));
          vertex_shader_compile_desc.shader_entry_point    = "main";
          vertex_shader_compile_desc.shader_feature_target = "vs_5_1";
          vertex_shader_compile_desc.shader_name           = "unlit_vertex";
          vertex_shader_compile_desc.shader_type           = ShaderType::VERTEX;

          return shader_library::compile_shader(vertex_shader_compile_desc);
        }

        wrl::ComPtr<ID3DBlob> load_unlit_vertex_shader()
        {
          // Load the vertex shader.
          wrl::ComPtr<ID3DBlob> vertex_shader_blob;
          if(DX_FAILED(D3DReadFileToBlob(L"unlit_vs.cso", &vertex_shader_blob)))
          {
            REX_ERROR(LogDirectX, "Failed to read compiled unlit vertex shader from file.");
            return nullptr;
          }

          return vertex_shader_blob;
        }

        wrl::ComPtr<ID3DBlob> compile_unlit_pixel_shader()
        {
          static const char* pixel_shader_code = "                          \
                                                                            \
          struct PixelShaderInput                                           \
          {                                                                 \
            float4 Color : COLOR;                                           \
          };                                                                \
                                                                            \
          float4 main(PixelShaderInput IN)                                  \
              : SV_Target                                                   \
          {                                                                 \
            return IN.Color;                                                \
          }";

          static shader_library::ShaderCompilationDesc pixel_shader_compile_desc;
          pixel_shader_compile_desc.shader_code           = memory::make_blob(pixel_shader_code, (s32)strlen(pixel_shader_code));
          pixel_shader_compile_desc.shader_entry_point    = "main";
          pixel_shader_compile_desc.shader_feature_target = "ps_5_1";
          pixel_shader_compile_desc.shader_name           = "unlit_pixel";
          pixel_shader_compile_desc.shader_type           = ShaderType::PIXEL;

          return shader_library::compile_shader(pixel_shader_compile_desc);
        }

        wrl::ComPtr<ID3DBlob> load_unlit_pixel_shader()
        {
          // Load the pixel shader.
          wrl::ComPtr<ID3DBlob> pixel_shader_blob;
          if(DX_FAILED(D3DReadFileToBlob(L"unlit_ps.cso", &pixel_shader_blob)))
          {
            REX_ERROR(LogDirectX, "Failed to read compiled unlit pixel shader from file.");
            return nullptr;
          }

          return pixel_shader_blob;
        }
      } // namespace internal

      rsl::unordered_map<rsl::string, ShaderInfo> g_shader_info;

      //-------------------------------------------------------------------------
      wrl::ComPtr<ID3DBlob> compile_shader(const ShaderCompilationDesc& desc)
      {
        s32 compile_flags = 0;
#ifdef REX_ENABLE_DEBUG_SHADER_COMPILATION
        compile_flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

        HRESULT hr = S_OK;

        wrl::ComPtr<ID3DBlob> byte_code = nullptr;
        wrl::ComPtr<ID3DBlob> errors    = nullptr;

        hr = D3DCompile2(desc.shader_code.data(), desc.shader_code.size(), desc.shader_name.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, desc.shader_entry_point.data(), desc.shader_feature_target.data(), compile_flags, 0, 0, 0, 0, &byte_code,
                         &errors);

        if(errors != nullptr)
        {
          REX_ERROR(LogDirectX, "{}", (char*)errors->GetBufferPointer());
          return nullptr;
        }

        if(DX_FAILED(hr))
        {
          REX_ERROR(LogDirectX, "Failed to compile shader");
          return nullptr;
        }

        return byte_code;
      }

      //-------------------------------------------------------------------------
      void load(Device* device)
      {
        g_shader_info[tags::unlit] = 
        {
            XMVertexPosCol::input_layout,

            internal::load_unlit_root_signature(device),

            internal::compile_unlit_vertex_shader(),
            internal::compile_unlit_pixel_shader()
        };
      }

      //-------------------------------------------------------------------------
      void unload()
      {
        g_shader_info.clear();
      }

      ShaderInfo find_shader_info(rsl::string_view name)
      {
        auto it = g_shader_info.find(name);
        if(it == g_shader_info.cend())
        {
          return {};
        }

        return it->value;
      }
    } // namespace shader_library
  }   // namespace renderer
} // namespace rex