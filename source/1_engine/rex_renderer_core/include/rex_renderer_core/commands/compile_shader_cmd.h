//#pragma once
//
//#include "rex_engine/memory/blob.h"
//#include "rex_renderer_core/resources/compile_shader.h"
//#include "rex_renderer_core/commands/render_cmd.h"
//#include "rex_renderer_core/shaders/shader_type.h"
//#include "rex_std/string_view.h"
//
//namespace rex
//{
//
//  namespace renderer
//  {
//    namespace commands
//    {
//      struct CompileShaderCommandDesc
//      {
//        rhi::ShaderType shader_type = rhi::ShaderType::None;
//        rsl::string_view shader_entry_point;
//        rsl::small_stack_string shader_name;
//        rsl::tiny_stack_string shader_feature_target;
//        memory::Blob shader_code;
//      };
//
//      rex::rhi::CompileShaderDesc create_compile_shader_parameters(rsl::string_view shaderName, rex::rhi::ShaderType shaderType, rsl::string_view filePath);
//
//      class CompileShader : public RenderCommand
//      {
//      public:
//        CompileShader(CompileShaderCommandDesc&& desc, const ResourceSlot& slot)
//            : RenderCommand()
//            , m_desc(rsl::move(desc))
//            , m_resource_slot(slot)
//        {
//        }
//
//        ~CompileShader() override = default;
//
//        bool execute() override
//        {
//          return backend::compile_shader(m_desc, m_resource_slot);
//        }
//
//      private:
//        CompileShaderCommandDesc m_desc;
//        ResourceSlot m_resource_slot;
//      };
//    } // namespace commands
//  }   // namespace renderer
//} // namespace rex