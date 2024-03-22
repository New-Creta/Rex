#include "rex_directx/system/directx_shader_compiler.h"
#include "rex_directx/diagnostics/log.h"
#include "rex_directx/diagnostics/directx_call.h"

#include "rex_renderer_core/resources/compile_shader.h"

#include <d3dcompiler.h>

namespace rex
{
  namespace rhi
  {
    DEFINE_LOG_CATEGORY(LogShaderCompiler, rex::LogVerbosity::Warning);

    wrl::ComPtr<ID3DBlob> ShaderCompiler::compile_shader(const CompileShaderDesc& desc)
    {
      s32 compile_flags = 0;
#ifdef REX_ENABLE_DEBUG_SHADER_COMPILATION
      compile_flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

      HRESULT hr = S_OK;

      wrl::ComPtr<ID3DBlob> byte_code = nullptr;
      wrl::ComPtr<ID3DBlob> errors = nullptr;

      hr = D3DCompile2(
        desc.shader_code.data(),
        desc.shader_code.size(),
        desc.shader_name.c_str(),
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        desc.shader_entry_point.data(),
        desc.shader_feature_target.data(),
        compile_flags, 0, 0, 0, 0, &byte_code, &errors);

      if (errors != nullptr)
      {
        REX_ERROR(LogShaderCompiler, "{}", (char*)errors->GetBufferPointer());
      }

      if (DX_FAILED(hr))
      {
        REX_ERROR(LogShaderCompiler, "Failed to compile shader");
      }
     
      return byte_code;
    }

  }
}