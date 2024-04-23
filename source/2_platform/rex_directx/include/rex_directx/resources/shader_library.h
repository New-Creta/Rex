#pragma once

#include "rex_directx/directx_util.h"
#include "rex_engine/memory/blob.h"
#include "rex_renderer_core/shader_type.h"
#include "rex_std/memory.h"
#include "rex_std/string_view.h"

namespace rex
{
  namespace renderer
  {
    class RootSignature;
    class Device;

    struct ShaderInfo
    {
      D3D12_INPUT_LAYOUT_DESC input_layout;

      rsl::shared_ptr<RootSignature> root_signature;

      wrl::ComPtr<ID3DBlob> vertex_shader;
      wrl::ComPtr<ID3DBlob> pixel_shader;
    };

    namespace shader_library
    {
      namespace root_parameters
      {
        // An enum for root signature parameters.
        // I'm not using scoped enums to avoid the explicit cast that would be required
        // to use these as root indices in the root signature.
        enum Unlit
        {
          MATRICES_CB, // ConstantBuffer<Mat> MatCB : register(b0);
          NUM_ROOT_PARAMETERS
        };
      } // namespace root_parameters

      namespace tags
      {
        static const rsl::string_view unlit = "unlit";
      }

      struct ShaderCompilationDesc
      {
        ShaderType shader_type = ShaderType::None;
        rsl::string_view shader_entry_point;
        rsl::small_stack_string shader_name;
        rsl::tiny_stack_string shader_feature_target;
        memory::Blob shader_code;
      };

      wrl::ComPtr<ID3DBlob> compile_shader(const ShaderCompilationDesc& desc);

      void load(Device* device);
      void unload();

      ShaderInfo find_shader_info(rsl::string_view name);
    } // namespace shader_library
  }   // namespace renderer
} // namespace rex