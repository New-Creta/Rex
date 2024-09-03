#include "rex_directx/system/dx_shader_compiler.h"

#include "rex_directx/diagnostics/dx_call.h"
#include "rex_engine/gfx/system/compile_shader.h"
#include "rex_engine/diagnostics/log.h"
#include "rex_engine/filesystem/vfs.h"
#include "rex_engine/filesystem/path.h"
#include "rex_engine/filesystem/file.h"

#include "rex_std/vector.h"
#include "rex_std/string_view.h"

#include <d3dcompiler.h>

namespace rex
{
  namespace gfx
  {
    DEFINE_LOG_CATEGORY(LogShaderCompiler);

    // This includes class allows the possibility of including files in shader code
    // This is useful if you want to share code between shaders
    class RexIncluder : public ID3DInclude
    {
    public:
      RexIncluder(rsl::vector<rsl::string_view>&& includePaths)
        : m_include_paths(rsl::move(includePaths))
      {
      }

      // Loop over the include paths and find the first valid path
      rsl::string find_file(rsl::string_view filename)
      {
        auto it = rsl::find_if(m_include_paths.cbegin(), m_include_paths.cend(),
        [&](rsl::string_view includePath)
        {
          return file::exists(path::join(includePath, filename));
        });

        return it != m_include_paths.cend()
        ? path::join(*it, filename)
        : rsl::string("");
      }

      STDMETHOD(Open)(D3D_INCLUDE_TYPE /*includeType*/, LPCSTR filename, LPCVOID /*parentData*/, LPCVOID* dataDoublePtr, uint32* bytesDoublePtr) override
      {
        rsl::string include_path = find_file(rsl::string_view(filename));
        if (include_path.empty())
        {
          return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
        }

        memory::Blob include_file_content = file::read_file(include_path);

        *dataDoublePtr = include_file_content.data();
        *bytesDoublePtr = static_cast<uint32>(include_file_content.size());

        m_include_map.emplace(*dataDoublePtr, rsl::move(include_file_content));

        return S_OK;
      }

      STDMETHOD(Close)(LPCVOID data) override
      {
        m_include_map.erase(data);

        return S_OK;
      }

      rsl::unordered_map<const void*, memory::Blob>   m_include_map;
      rsl::vector<rsl::string_view> m_include_paths;
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

      RexIncluder includer({
        vfs::mount_path(MountingPoint::EngineShaders)
      });

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
      else if(DX_FAILED(hr))
      {
        REX_ERROR(LogShaderCompiler, "Failed to compile shader");
      }

      return byte_code;
    }

  } // namespace gfx
} // namespace rex