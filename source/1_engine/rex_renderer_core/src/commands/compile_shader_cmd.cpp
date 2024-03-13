#include "rex_renderer_core/commands/compile_shader_cmd.h"

#include "rex_engine/filesystem/vfs.h"

namespace rex
{
  namespace renderer
  {
    namespace commands
    {
      rex::renderer::commands::CompileShaderCommandDesc create_compile_shader_parameters(const rsl::small_stack_string& shaderName, rex::renderer::ShaderType shaderType, rsl::string_view filePath)
      {
        rex::renderer::commands::CompileShaderCommandDesc compile_shader_command_desc{};

        switch (shaderType)
        {
        case rex::renderer::ShaderType::VERTEX:
          compile_shader_command_desc.shader_entry_point = "VS";
          compile_shader_command_desc.shader_feature_target = "vs_5_1";
          break;
        case rex::renderer::ShaderType::PIXEL:
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
}