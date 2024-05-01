#include "rex_renderer_core/resources/compile_shader.h"

#include "rex_engine/filesystem/vfs.h"

namespace rex
{
  namespace rhi
  {
    CompileShaderDesc::CompileShaderDesc(rsl::string_view shaderName, rex::rhi::ShaderType shaderType, rsl::string_view filePath)
    {
      switch(shaderType)
      {
        case rex::rhi::ShaderType::Vertex:
          shader_entry_point    = "VS";
          shader_feature_target = "vs_5_1";
          break;
        case rex::rhi::ShaderType::Pixel:
          shader_entry_point    = "PS";
          shader_feature_target = "ps_5_1";
          break;
        default: break;
      }

      shader_code = rex::vfs::read_file(filePath);
      shader_name = shaderName;
      shader_type = shaderType;
    }
  } // namespace rhi
} // namespace rex