#include "rex_renderer_core/rendering/scene.h"

#include "rex_engine/app/core_application.h"
#include "rex_engine/app/windowinfo.h"
#include "rex_renderer_core/rendering/cull_mode.h"
#include "rex_renderer_core/rendering/depth_info.h"
#include "rex_renderer_core/rendering/vertex.h"
#include "rex_renderer_core/system/rhi.h"

#include <glm/gtc/matrix_transform.hpp>

namespace rex
{
  namespace renderer
  {
    struct ObjectConstants
    {
      glm::mat4 world = glm::mat4(1.0f);
    };

    Scene::Scene()
    {
      build_shader("regina\\Shaders\\color.hlsl", "regina\\Shaders\\color.hlsl");
      build_input_layout();

      build_raster_state();
      build_pso();

      use_pso();
    }

    void Scene::update()
    {
      use_shader();
      use_pso();

      update_object_constant_buffers();
    }

    void Scene::build_shader(rsl::string_view vertexShaderPath, rsl::string_view pixelShaderPath)
    {
      rex::rhi::CompileShaderDesc vs_compile_command_desc = rex::rhi::CompileShaderDesc("standardVS"_small, rex::rhi::ShaderType::VERTEX, vertexShaderPath);
      rex::rhi::CompileShaderDesc ps_compile_command_desc = rex::rhi::CompileShaderDesc("opaquePS"_small, rex::rhi::ShaderType::PIXEL, pixelShaderPath);

      rex::rhi::LinkShaderDesc link_shader_desc;
      link_shader_desc.vertex_shader = rhi::compile_shader(rsl::move(vs_compile_command_desc));
      link_shader_desc.pixel_shader  = rhi::compile_shader(rsl::move(ps_compile_command_desc));
      link_shader_desc.constants     = rsl::make_unique<rex::rhi::ConstantLayoutDescription[]>(2);
      link_shader_desc.constants[0]  = {rex::rhi::ConstantType::CBuffer, "ObjectConstants", 0};
      link_shader_desc.constants[1]  = {rex::rhi::ConstantType::CBuffer, "PassConstants", 1};

      m_shader_program = rex::rhi::link_shader(link_shader_desc);
    }

    void Scene::build_input_layout()
    {
      m_input_layout = rex::rhi::create_input_layout(rex::renderer::VertexPosNormCol::layout());
    }

    void Scene::build_raster_state(FillMode fillMode)
    {
      rex::rhi::RasterStateDesc solid_rs_command_desc;
      solid_rs_command_desc.fill_mode = fillMode;
      solid_rs_command_desc.cull_mode = rex::renderer::CullMode::BACK;
      m_raster_state                  = rex::rhi::create_raster_state(solid_rs_command_desc);
    }

    void Scene::build_pso()
    {
      rex::rhi::PipelineStateDesc pso_desc;
      pso_desc.input_layout = m_input_layout;
      pso_desc.raster_state = m_raster_state;
      pso_desc.shader       = m_shader_program;

      m_pso = rex::rhi::create_pso(pso_desc);
    }

    void Scene::use_shader()
    {
      // We have a global shader for the scene at the moment
      // In the future, this will be set on an individual model
      rex::renderer::set_shader(m_shader_program);
    }
    void Scene::use_pso()
    {
      rex::renderer::set_pso(m_pso);
    }

    void Scene::update_view()
    {
      // Convert Spherical to Cartesian coordinates.
      m_eye_pos.x = 15.0f * sinf(0.2f * glm::pi<f32>()) * cosf(1.5f * glm::pi<f32>());
      m_eye_pos.y = 15.0f * cosf(0.2f * glm::pi<f32>());
      m_eye_pos.z = 35.0f * sinf(0.2f * glm::pi<f32>()) * sinf(1.5f * glm::pi<f32>());

      // Build the view matrix.
      const glm::vec3 pos    = glm::vec3(m_eye_pos.x, m_eye_pos.y, m_eye_pos.z);
      const glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);
      const glm::vec3 up     = glm::vec3(0.0f, 1.0f, 0.0f);

      m_view = glm::lookAt(pos, target, up);
      m_view = glm::transpose(m_view); // DirectX backend ( so we have to transpose, expects row major matrices )
    }
  }                                    // namespace renderer
} // namespace rex