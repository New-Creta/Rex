#include "rex_renderer_core/resources/compile_shader.h"

#include "rex_engine/filesystem/vfs.h"

namespace rex
{
  namespace rhi
  {
    rex::rhi::CompileShaderDesc create_compile_shader_parameters(rsl::string_view shaderName, rex::rhi::ShaderType shaderType, rsl::string_view filePath)
    {
      rex::rhi::CompileShaderDesc compile_shader_command_desc{};

      switch (shaderType)
      {
      case rex::rhi::ShaderType::VERTEX:
        compile_shader_command_desc.shader_entry_point = "VS";
        compile_shader_command_desc.shader_feature_target = "vs_5_1";
        break;
      case rex::rhi::ShaderType::PIXEL:
        compile_shader_command_desc.shader_entry_point = "PS";
        compile_shader_command_desc.shader_feature_target = "ps_5_1";
        break;
      default: break;
      }

      compile_shader_command_desc.shader_code = rex::vfs::read_file(filePath);
      compile_shader_command_desc.shader_name = shaderName;
      compile_shader_command_desc.shader_type = shaderType;

      return compile_shader_command_desc;
    }
  }
}