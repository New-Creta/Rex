#pragma once

#include "rex_renderer_core/commands/render_cmd.h"
#include "rex_renderer_core/shader_type.h"

#include <rex_std/bonus/string/stack_string.h>

namespace rex
{
  namespace renderer
  {
    namespace commands
    {
      static rsl::tiny_stack_string s_vertex_entry_point   = "VS";
      static rsl::tiny_stack_string s_pixel_entry_point    = "PS";
      static rsl::tiny_stack_string s_geometry_entry_point = "GS";
      static rsl::tiny_stack_string s_compute_entry_point  = "CS";

      struct CompileShaderCommandDesc
      {
        RenderCommandDesc command;

        ShaderType shader_type;
        rsl::small_stack_string shader_name;
        rsl::tiny_stack_string shader_entry_point;
        rsl::tiny_stack_string shader_feature_target;
        char8* shader_code;
        s64 shader_code_size;
      };

      class CompileShader : public RenderCommand
      {
      public:
        CompileShader(CompileShaderCommandDesc&& desc)
            : RenderCommand(rsl::move(desc.command))
            , m_desc(rsl::move(desc))
        {
          switch(m_desc.shader_type)
          {
            case ShaderType::VERTEX: m_desc.shader_entry_point = s_vertex_entry_point.data(); break;
            case ShaderType::PIXEL: m_desc.shader_entry_point = s_pixel_entry_point.data(); break;
            case ShaderType::GEOMETRY: m_desc.shader_entry_point = s_geometry_entry_point.data(); break;
            case ShaderType::COMPUTE: m_desc.shader_entry_point = s_compute_entry_point.data(); break;
          }
        }

        ~CompileShader() override = default;

        bool execute() override
        {
          result = backend::compile_shader(cmd.compile_shader_params, cmd.resource_slot);
          memory_free(cmd.compile_shader_params.shader_code);

          return result;
        }

      private:
        CompileShaderCommandDesc m_desc;
      };
    } // namespace commands
  }   // namespace renderer
} // namespace rex