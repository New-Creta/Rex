#include "rex_renderer_core/rendering/scene.h"

#include "rex_renderer_core/rendering/scene_renderer.h"
#include "rex_renderer_core/rendering/cull_mode.h"
#include "rex_renderer_core/rendering/default_targets_info.h"
#include "rex_renderer_core/rendering/default_depth_info.h"
#include "rex_renderer_core/commands/update_committed_resource_cmd.h"
#include "rex_renderer_core/commands/compile_shader_cmd.h"
#include "rex_renderer_core/commands/create_input_layout_cmd.h"
#include "rex_renderer_core/commands/create_raster_state_cmd.h"
#include "rex_renderer_core/commands/create_pipeline_state_cmd.h"
#include "rex_renderer_core/commands/link_shader_cmd.h"

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
    //-------------------------------------------------------------------------
    void Scene::clear_render_items()
    {
      m_render_items.clear();
    }

    void Scene::render(rex::renderer::SceneRenderer* renderer, f32 width, f32 height)
    {
      // This is a fancy way of saying "Get the resource slot of the gpu resource that's already on the gpu, which has memory available for the objects constant buffers"
      const rex::renderer::ResourceSlot curr_object_cr = get_object_committed_resource_of_frame(rex::renderer::active_frame());

      // Update view needs to be done through the camera
      // We don't have a camera class yet, it's static anyway
      // so we can just do it in the scene at the moment
      update_view();
      update_object_constant_buffers(curr_object_cr);
      update_pass_constant_buffers(width, height);

      use_shader();

      // TODO: The following needs to be cleaned up, setting the constant buffer needs to be done better
      rex::renderer::ResourceSlot const curr_pass_cb = get_active_pass_constant_buffer_for_frame(rex::renderer::active_frame()->slot_id());
      rex::renderer::set_constant_buffer_view(curr_pass_cb, 1);

      rsl::vector<rex::renderer::ResourceSlot> contant_buffers = get_active_constant_buffer_for_frame(rex::renderer::active_frame()->slot_id());
      render_items(renderer, contant_buffers);

      // This also needs to be done by the camera, but like before
      // We don't have a camera class at the moment and it's static anyway
      // So we let the scene handle it at the moment.
      // The window resized, so update the aspect ratio and recompute the projection matrix.
      const auto fov = 0.25f * glm::pi<f32>();
      //const auto width = static_cast<f32>(rex::globals::window_info().width);
      //const auto height = static_cast<f32>(rex::globals::window_info().height);
      const auto near_plane = rex::globals::default_depth_info().near_plane;
      const auto far_plane = rex::globals::default_depth_info().far_plane;
      m_proj = glm::perspectiveFov(fov, width, height, near_plane, far_plane);
      m_proj = glm::transpose(m_proj); // DirectX backend ( so we have to transpose, expects row major matrices )    }
    }

    //-------------------------------------------------------------------------
    u32 Scene::render_item_count() const
    {
      return m_render_items.size();
    }

    void Scene::build_shader(rsl::string_view vertexShaderPath, rsl::string_view pixelShaderPath)
    {
      rex::renderer::commands::CompileShaderCommandDesc vs_compile_command_desc = rex::renderer::commands::create_compile_shader_parameters("standardVS"_small, rex::renderer::ShaderType::VERTEX, vertexShaderPath);
      rex::renderer::commands::CompileShaderCommandDesc ps_compile_command_desc = rex::renderer::commands::create_compile_shader_parameters("opaquePS"_small, rex::renderer::ShaderType::PIXEL, pixelShaderPath);

      rex::renderer::commands::LinkShaderCommandDesc link_shader_command_desc;
      link_shader_command_desc.vertex_shader = rhi::compile_shader(rsl::move(vs_compile_command_desc));
      link_shader_command_desc.pixel_shader = rhi::compile_shader(rsl::move(ps_compile_command_desc));
      link_shader_command_desc.constants = 
      { 
        rex::renderer::commands::ConstantLayoutDescription 
        {
          rex::renderer::commands::ConstantType::CBuffer, "ObjectConstants", 0
        },
        rex::renderer::commands::ConstantLayoutDescription 
        {
          rex::renderer::commands::ConstantType::CBuffer, "PassConstants", 1
        } 
      };

      m_shader_program = rex::renderer::link_shader(rsl::move(link_shader_command_desc));
    }

    void Scene::build_input_layout()
    {
      rex::renderer::commands::CreateInputLayoutCommandDesc input_layout_command_desc;
      input_layout_command_desc.input_layout = 
      { 
        rex::renderer::commands::InputLayoutDescription 
        {
          "POSITION", 0, rex::renderer::VertexBufferFormat::FLOAT3, 0, 0, rex::renderer::InputLayoutClassification::PerVertexData, 0
        },
        rex::renderer::commands::InputLayoutDescription 
        {
          "COLOR", 0, rex::renderer::VertexBufferFormat::FLOAT4, 0, 12, rex::renderer::InputLayoutClassification::PerVertexData, 0
        }
      };

      m_input_layout = rex::renderer::create_input_layout(rsl::move(input_layout_command_desc));
    }

    void Scene::build_raster_state(FillMode fillMode)
    {
      rex::renderer::commands::CreateRasterStateCommandDesc solid_rs_command_desc;
      solid_rs_command_desc.fill_mode = fillMode;
      solid_rs_command_desc.cull_mode = rex::renderer::CullMode::BACK;
      m_raster_state = rex::renderer::create_raster_state(rsl::move(solid_rs_command_desc));
    }

    void Scene::build_frame_resources()
    {
      for (int i = 0; i < rex::renderer::max_frames_in_flight(); ++i)
      {
        rex::renderer::ResourceSlot frame_slot = rex::renderer::create_frame_resource();

        m_frame_resource_data.emplace_back(frame_slot);
      }
    }
    void Scene::build_constant_buffers()
    {
      const s32 num_render_items = render_item_count();

      // Attach committed resources required for each frame
      // The committed resources contain the uploader for each constant buffer that is created for a specific frame
      // The uploader needs to be kept alive at least until the commandlist is executed,
      //      hence why we store it in a separate resource so the resource pools keeps a reference to it
      // This internal knowledge should move to the renderer itself, as it can create the uploader for a CBV
      //      when a CBV allocation is requested.
      for (s32 frame = 0; frame < rex::renderer::max_frames_in_flight(); ++frame)
      {
        m_frame_resource_data[frame].set_object_committed_resource_slot(frame, num_render_items, sizeof(ObjectConstants)); // This basicallly says "create upload buffer for this frame with size = (count * size per object)
        m_frame_resource_data[frame].set_pass_committed_resource_slot(frame, 1, sizeof(PassConstants)); // This basically says "create upload buffer for this frame with size = (count * size per pass)
      }

      // Need a CBV descriptor for each object for each frame resource.
      for (s32 frame = 0; frame < rex::renderer::max_frames_in_flight(); ++frame)
      {
        for (s32 i = 0; i < num_render_items; ++i)
        {
          // This goes over the upload buffer for the constant buffers per object, creates a view to a range holding 1 "ObjectConstants" and increments the start address for the next
          // In my opinion, this is a bad design, there are too many side effects here
          m_frame_resource_data[frame].add_object_constant_buffer_slot(frame, m_frame_resource_data[frame].object_committed_resource_slot(), sizeof(ObjectConstants));
        }
      }

      // Last three descriptors are the pass CBVs for each frame resource.
      for (s32 frame = 0; frame < rex::renderer::max_frames_in_flight(); ++frame)
      {
        // This goes over the upload buffer for the constant buffers per object, creates a view to a range holding 1 "PassConstants" and increments the start address for the next
          // In my opinion, this is a bad design, there are too many side effects here
        m_frame_resource_data[frame].set_pass_constant_buffer_slot(frame, m_frame_resource_data[frame].pass_committed_resource_slot(), sizeof(PassConstants));
      }
    }
    void Scene::build_pso()
    {
      // Pipeline state object has to be aware of the render targets
      // This is because it needs to query the render target texture format
      // If there are no render targets set no PSO can be created
      rex::renderer::set_render_targets(rex::globals::default_targets_info().back_buffer_color, rex::globals::default_targets_info().depth_buffer);

      rex::renderer::commands::CreatePipelineStateCommandDesc create_pso_command_desc;
      create_pso_command_desc.input_layout = m_input_layout;
      create_pso_command_desc.num_render_targets = 1;
      create_pso_command_desc.shader_program = m_shader_program;
      create_pso_command_desc.rasterizer_state = m_raster_state;

      m_pso = rex::renderer::create_pipeline_state_object(rsl::move(create_pso_command_desc));
    }

    void Scene::update_object_constant_buffers(const rex::renderer::ResourceSlot& currObjectCommittedResource)
    {
      for (const auto& render_item : m_render_items)
      {
        // Only update the CBuffer data if the constants have changed.
        // This needs to be tracked per frame resource.
        if (render_item.num_frames_dirty > 0)
        {
          // Assign the new world matrixz
          ObjectConstants obj_constants;
          obj_constants.world = glm::transpose(render_item.world); // DirectX backend ( so we have to transpose, expects row major matrices )

          rex::renderer::commands::UpdateCommittedResourceCommandDesc update_constant_buffer_command_desc;
          update_constant_buffer_command_desc.element_index = render_item.constant_buffer_index;
          update_constant_buffer_command_desc.buffer_data = rex::memory::make_blob(reinterpret_cast<rsl::byte*>(&obj_constants), rsl::memory_size(sizeof(ObjectConstants)));

          rex::renderer::update_committed_resource(rsl::move(update_constant_buffer_command_desc), currObjectCommittedResource);
        }
      }
    }
    void Scene::render_items(SceneRenderer* renderer, const rsl::vector<ResourceSlot>& contantBuffers) const
    {
      for (const auto& render_item : m_render_items)
      {
        rex::renderer::set_vertex_buffer(render_item.geometry->vertex_buffer_slot(), 0, render_item.geometry->vertex_buffer_byte_stride(), 0);
        rex::renderer::set_index_buffer(render_item.geometry->index_buffer_slot(), render_item.geometry->index_buffer_format(), 0);
        rex::renderer::set_primitive_topology(rex::renderer::PrimitiveTopology::TRIANGLELIST);

        rex::renderer::ResourceSlot const curr_object_cb = contantBuffers[render_item.constant_buffer_index];
        rex::renderer::set_constant_buffer_view(curr_object_cb, 0);

        rex::renderer::renderer_draw_indexed_instanced(1, 0, render_item.index_count, render_item.start_index_location, render_item.base_vertex_location);
      }

    }
    void Scene::use_shader()
    {
      // We have a global shader for the scene at the moment
      // In the future, this will be set on an individual model
      rex::renderer::set_shader(m_shader_program);
    }
    void Scene::use_pso()
    {
      rex::renderer::set_pipeline_state_object(m_pso);
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
      rex::renderer::ResourceSlot const curr_pass_cr = get_pass_committed_resource_of_frame(rex::renderer::active_frame());

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

      rex::renderer::commands::UpdateCommittedResourceCommandDesc update_constant_buffer_command_desc;
      update_constant_buffer_command_desc.element_index = 0;
      update_constant_buffer_command_desc.buffer_data = rex::memory::make_blob(reinterpret_cast<rsl::byte*>(&m_pass_constants), rsl::memory_size(sizeof(PassConstants)));

      rex::renderer::update_committed_resource(rsl::move(update_constant_buffer_command_desc), curr_pass_cr);
    }

    //-------------------------------------------------------------------------
    rex::renderer::ResourceSlot Scene::get_object_committed_resource_of_frame(const rex::renderer::ResourceSlot* frame)
    {
      auto it = rsl::find_if(m_frame_resource_data.cbegin(), m_frame_resource_data.cend(), [frame](const FrameData& data) { return *frame == data.frame_slot(); });

      return it != m_frame_resource_data.cend() ? it->object_committed_resource_slot() : rex::renderer::ResourceSlot::make_invalid();
    }
    //-------------------------------------------------------------------------
    rex::renderer::ResourceSlot Scene::get_pass_committed_resource_of_frame(const rex::renderer::ResourceSlot* frame)
    {
      auto it = rsl::find_if(m_frame_resource_data.cbegin(), m_frame_resource_data.cend(), [frame](const FrameData& data) { return *frame == data.frame_slot(); });

      return it != m_frame_resource_data.cend() ? it->pass_committed_resource_slot() : rex::renderer::ResourceSlot::make_invalid();
    }
    //-------------------------------------------------------------------------
    rsl::vector<rex::renderer::ResourceSlot> Scene::get_active_constant_buffer_for_frame(s32 frame)
    {
      auto it = rsl::find_if(m_frame_resource_data.cbegin(), m_frame_resource_data.cend(), [frame](const FrameData& data) { return frame == data.frame_slot().slot_id(); });
      return it != m_frame_resource_data.cend() ? it->object_constant_buffer_slots() : rsl::vector<rex::renderer::ResourceSlot>{};
    }
    //-------------------------------------------------------------------------
    rex::renderer::ResourceSlot Scene::get_active_pass_constant_buffer_for_frame(s32 frame)
    {
      auto it = rsl::find_if(m_frame_resource_data.cbegin(), m_frame_resource_data.cend(), [frame](const FrameData& data) { return frame == data.frame_slot().slot_id(); });

      return it != m_frame_resource_data.cend() ? it->pass_constant_buffer_slot() : rex::renderer::ResourceSlot::make_invalid();
    }
  } // namespace renderer
} // namespace rex