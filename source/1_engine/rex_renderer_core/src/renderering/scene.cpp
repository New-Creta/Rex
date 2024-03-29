#include "rex_renderer_core/rendering/scene.h"

#include "rex_renderer_core/rendering/scene_renderer.h"
#include "rex_renderer_core/rendering/cull_mode.h"
#include "rex_renderer_core/rendering/default_targets_info.h"
#include "rex_renderer_core/rendering/default_depth_info.h"

#include "rex_renderer_core/system/rhi.h"

#include "rex_engine/app/core_application.h"
#include "rex_engine/app/windowinfo.h"

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
      // Nothing to do
    }

    //-------------------------------------------------------------------------
    void Scene::add_render_item(RenderItem&& item)
    {
      m_render_items.push_back(rsl::move(item));
    }

    void Scene::update()
    {
      use_shader();
      use_pso();

      update_object_constant_buffers();

      rhi::set_constant_buffer(1, m_pass_cb);
    }

    void Scene::build_shader(rsl::string_view vertexShaderPath, rsl::string_view pixelShaderPath)
    {
      rex::rhi::CompileShaderDesc vs_compile_command_desc = rex::rhi::create_compile_shader_parameters("standardVS"_small, rex::rhi::ShaderType::VERTEX, vertexShaderPath);
      rex::rhi::CompileShaderDesc ps_compile_command_desc = rex::rhi::create_compile_shader_parameters("opaquePS"_small, rex::rhi::ShaderType::PIXEL, pixelShaderPath);

      rex::rhi::LinkShaderDesc link_shader_desc;
      link_shader_desc.vertex_shader = rhi::compile_shader(rsl::move(vs_compile_command_desc));
      link_shader_desc.pixel_shader = rhi::compile_shader(rsl::move(ps_compile_command_desc));
      link_shader_desc.constants = rsl::make_unique<rex::rhi::ConstantLayoutDescription[]>(2);
      link_shader_desc.constants[0] = { rex::rhi::ConstantType::CBuffer, "ObjectConstants", 0 };
      link_shader_desc.constants[1] = { rex::rhi::ConstantType::CBuffer, "PassConstants", 1 };

      m_shader_program = rex::rhi::link_shader(link_shader_desc);
    }

    void Scene::build_input_layout()
    {
      rex::rhi::InputLayoutDesc input_layout_command_desc;
      input_layout_command_desc.input_layout = 
      { 
        rex::rhi::InputLayoutElementDesc 
        {
          "POSITION",  rex::renderer::VertexBufferFormat::FLOAT3, rex::renderer::InputLayoutClassification::PerVertexData, 0, 0, 0, 0
        },
        rex::rhi::InputLayoutElementDesc
        {
          "COLOR", rex::renderer::VertexBufferFormat::FLOAT4, rex::renderer::InputLayoutClassification::PerVertexData, 0, 0, 12, 0
        }
      };

      m_input_layout = rex::rhi::create_input_layout(input_layout_command_desc);
    }

    void Scene::build_raster_state(FillMode fillMode)
    {
      rex::rhi::RasterStateDesc solid_rs_command_desc;
      solid_rs_command_desc.fill_mode = fillMode;
      solid_rs_command_desc.cull_mode = rex::renderer::CullMode::BACK;
      m_raster_state = rex::rhi::create_raster_state(solid_rs_command_desc);
    }

    void Scene::build_constant_buffers(f32 width, f32 height)
    {
      m_pass_constants.eye_pos_w.x = 15.0f * sinf(0.2f * glm::pi<f32>()) * cosf(1.5f * glm::pi<f32>());
      m_pass_constants.eye_pos_w.y = 15.0f * cosf(0.2f * glm::pi<f32>());
      m_pass_constants.eye_pos_w.z = 35.0f * sinf(0.2f * glm::pi<f32>()) * sinf(1.5f * glm::pi<f32>());

      const glm::vec3 pos = glm::vec3(m_pass_constants.eye_pos_w.x, m_pass_constants.eye_pos_w.y, m_pass_constants.eye_pos_w.z);
      const glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);
      const glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

      glm::mat4 view = glm::lookAt(pos, target, up);
      view = glm::transpose(view); // DirectX backend ( so we have to transpose, expects row major matrices )

      glm::mat4 proj = glm::perspectiveFov(0.25f * glm::pi<f32>(), width, height, rex::globals::default_depth_info().near_plane, rex::globals::default_depth_info().far_plane);
      proj = glm::transpose(proj); // DirectX backend ( so we have to transpose, expects row major matrices )

      const glm::mat4 view_proj = view * proj;

      const glm::mat4 inv_view = glm::inverse(view);
      const glm::mat4 inv_proj = glm::inverse(proj);
      const glm::mat4 inv_view_proj = glm::inverse(view_proj);

      m_pass_constants.view = view;
      m_pass_constants.inv_view = inv_view;
      m_pass_constants.proj = proj;
      m_pass_constants.inv_proj = inv_proj;
      m_pass_constants.view_proj = view_proj;
      m_pass_constants.inv_view_proj = inv_view_proj;

      m_pass_constants.render_target_size = glm::vec2(width, height);
      m_pass_constants.inv_render_target_size = glm::vec2(1.0f / width, 1.0f / height);
      m_pass_constants.near_z = rex::globals::default_depth_info().near_plane;
      m_pass_constants.far_z = rex::globals::default_depth_info().far_plane;
      m_pass_constants.delta_time = rex::globals::frame_info().delta_time().to_seconds();

      ConstantBufferDesc desc;
      desc.blob.allocate(rsl::memory_size(sizeof(m_pass_constants)));
      desc.blob.write(&m_pass_constants, rsl::memory_size(sizeof(m_pass_constants)));
      m_pass_cb = rhi::create_constant_buffer(desc);
    }
    
    void Scene::build_pso()
    {
      // Pipeline state object has to be aware of the render targets
      // This is because it needs to query the render target texture format
      // If there are no render targets set no PSO can be created
      //rex::renderer::set_render_targets(rex::globals::default_targets_info().back_buffer_color, rex::globals::default_targets_info().depth_buffer);

      rex::rhi::PipelineStateDesc pso_desc;
      pso_desc.input_layout = m_input_layout;
      pso_desc.raster_state = m_raster_state;
      pso_desc.shader = m_shader_program;      

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
      const glm::vec3 pos = glm::vec3(m_eye_pos.x, m_eye_pos.y, m_eye_pos.z);
      const glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);
      const glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

      m_view = glm::lookAt(pos, target, up);
      m_view = glm::transpose(m_view); // DirectX backend ( so we have to transpose, expects row major matrices )
    }

    void Scene::update_pass_constant_buffers(f32 width, f32 height)
    {
      const glm::mat4& view = m_view;
      const glm::mat4& proj = m_proj;

      const glm::mat4 view_proj = view * proj;

      const glm::mat4 inv_view = glm::inverse(view);
      const glm::mat4 inv_proj = glm::inverse(proj);
      const glm::mat4 inv_view_proj = glm::inverse(view_proj);

      m_pass_constants.view = view;
      m_pass_constants.inv_view = inv_view;
      m_pass_constants.proj = proj;
      m_pass_constants.inv_proj = inv_proj;
      m_pass_constants.view_proj = view_proj;
      m_pass_constants.inv_view_proj = inv_view_proj;

      m_pass_constants.eye_pos_w = m_eye_pos;
      m_pass_constants.render_target_size = glm::vec2(width, height);
      m_pass_constants.inv_render_target_size = glm::vec2(1.0f / width, 1.0f / height);
      m_pass_constants.near_z = rex::globals::default_depth_info().near_plane;
      m_pass_constants.far_z = rex::globals::default_depth_info().far_plane;
      m_pass_constants.delta_time = rex::globals::frame_info().delta_time().to_seconds();
    }
  } // namespace renderer
} // namespace rex