#include "rex_directx/system/dx_shader_compiler.h"

#include "rex_directx/diagnostics/dx_call.h"
#include "rex_renderer_core/system/compile_shader.h"
#include "rex_engine/diagnostics/log.h"
#include "rex_engine/filesystem/vfs.h"
#include "rex_engine/filesystem/path.h"
#include "rex_engine/filesystem/file.h"

#include <d3dcompiler.h>

namespace rex
{
  namespace gfx
  {
    DEFINE_LOG_CATEGORY(LogShaderCompiler);

    class RexIncluder final : public ID3DInclude
    {
    public:
      ~RexIncluder()
      {
      }

      STDMETHOD(Open)(D3D_INCLUDE_TYPE IncludeType, LPCSTR filename, LPCVOID pParentData, LPCVOID* ppData, uint32* pBytes) override
      {
        rsl::string include_path = path::join(vfs::mount_path(MountingPoint::EngineShaders), filename);
        memory::Blob include_file_content = file::read_file(include_path);

        *ppData = include_file_content.data();
        *pBytes = include_file_content.size();

        m_include_map.emplace(*ppData, rsl::move(include_file_content));

        return S_OK;
      }

      STDMETHOD(Close)(LPCVOID pData) override
      {
        m_include_map.erase(pData);

        return S_OK;
      }

      rsl::unordered_map<void*, memory::Blob>   m_include_map;
    };

    wrl::ComPtr<ID3DBlob> ShaderCompiler::compile_shader(const CompileShaderDesc& desc) // NOLINT(readability-convert-member-functions-to-static)
    {
      s32 compile_flags = 0;
#ifdef REX_ENABLE_DEBUG_SHADER_COMPILATION
      compile_flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

      HRESULT hr = S_OK;

      wrl::ComPtr<ID3DBlob> byte_code = nullptr;
      wrl::ComPtr<ID3DBlob> errors    = nullptr;

      RexIncluder includer;

      // clang-format off
      hr = D3DCompile2(
        desc.shader_source_code.data(),
        desc.shader_source_code.size(),
        desc.shader_name.size()  // Need to check here as an empty string results in an compiler error
        ? desc.shader_name.data()
        : nullptr,
        nullptr,
        &includer,
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