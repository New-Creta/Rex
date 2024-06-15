#include "rex_renderer_core/system/compile_shader.h"

#include "rex_engine/filesystem/vfs.h"

namespace rex
{
  namespace gfx
  {
    CompileShaderDesc::CompileShaderDesc(rsl::string_view shaderName, ShaderType shaderType, rsl::string_view filePath)
    {
      switch(shaderType)
      {
        case ShaderType::Vertex:
          shader_entry_point    = "VS";
          shader_feature_target = "vs_5_1";
          break;
        case ShaderType::Pixel:
          shader_entry_point    = "PS";
          shader_feature_target = "ps_5_1";
          break;
        default: break;
      }

      shader_code = vfs::read_file(filePath);
      shader_name = shaderName;
      shader_type = shaderType;
    }
  } // namespace gfx
} // namespace rex