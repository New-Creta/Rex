#include "rex_directx/system/dx_shader_compiler.h"

#include "rex_directx/diagnostics/dx_call.h"
#include "rex_renderer_core/system/compile_shader.h"
#include "rex_engine/diagnostics/log.h"

#include <d3dcompiler.h>

namespace rex
{
  namespace gfx
  {
    DEFINE_LOG_CATEGORY(LogShaderCompiler);

    wrl::ComPtr<ID3DBlob> ShaderCompiler::compile_shader(const CompileShaderDesc& desc) // NOLINT(readability-convert-member-functions-to-static)
    {
      s32 compile_flags = 0;
#ifdef REX_ENABLE_DEBUG_SHADER_COMPILATION
      compile_flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

      HRESULT hr = S_OK;

      wrl::ComPtr<ID3DBlob> byte_code = nullptr;
      wrl::ComPtr<ID3DBlob> errors    = nullptr;

      // clang-format off
      hr = D3DCompile2(
        desc.shader_code.data(),
        desc.shader_code.size(),
        desc.shader_name.c_str(),
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        desc.shader_entry_point.data(),
        desc.shader_feature_target.data(),
        compile_flags, 0, 0, nullptr, 0, &byte_code, &errors);
      // clang-format on

      if(errors != nullptr)
      {
        REX_ERROR(LogShaderCompiler, "{}", (char*)errors->GetBufferPointer()); // NOLINT(cppcoreguidelines-pro-type-cstyle-cast)
      }

      if(DX_FAILED(hr))
      {
        REX_ERROR(LogShaderCompiler, "Failed to compile shader");
      }

      return byte_code;
    }

  } // namespace gfx
} // namespace rex