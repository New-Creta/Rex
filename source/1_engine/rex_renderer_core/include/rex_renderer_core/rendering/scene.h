#pragma once

#include "rex_renderer_core/rendering/render_item.h"
#include "rex_renderer_core/rendering/fill_mode.h"
#include "rex_renderer_core/rendering/frame_data.h"
#include "rex_std/vector.h"

#include "rex_renderer_core/resource_management/resource.h"
#include "rex_renderer_core/commands/attach_committed_resource_to_frame_cmd.h"
#include "rex_renderer_core/commands/create_constant_buffer_cmd.h"

namespace rex
{
  namespace renderer
  {
    class SceneRenderer;

    struct PassConstants
    {
      glm::mat4 view = glm::mat4(1.0f);
      glm::mat4 inv_view = glm::mat4(1.0f);
      glm::mat4 proj = glm::mat4(1.0f);
      glm::mat4 inv_proj = glm::mat4(1.0f);
      glm::mat4 view_proj = glm::mat4(1.0f);
      glm::mat4 inv_view_proj = glm::mat4(1.0f);

      glm::vec3 eye_pos_w = { 0.0f, 0.0f, 0.0f };
      f32 cb_padding_1 = 0.0f;

      glm::vec2 render_target_size = { 0.0f, 0.0f };
      glm::vec2 inv_render_target_size = { 0.0f, 0.0f };

      f32 near_z = 0.0f;
      f32 far_z = 0.0f;

      f32 total_time = 0.0f;
      f32 delta_time = 0.0f;
    };

    class Scene
    {
    public:
      using RenderItems       = rsl::vector<RenderItem>;
      using RenderItemIt      = RenderItems::iterator;
      using ConstRenderItemIt = RenderItems::const_iterator;

      Scene();

      void add_render_item(RenderItem&& item);
      void clear_render_items();

      void render(SceneRenderer* renderer, f32 width, f32 height);
      u32 render_item_count() const;

    protected:
      // Initialization
      void build_shader(rsl::string_view vertexShaderPath, rsl::string_view pixelShaderPath);
      void build_input_layout();
      void build_raster_state(FillMode fillMode = FillMode::SOLID);
      void build_pso();
      void build_frame_resources();
      void build_constant_buffers();

      // Update
      void update_pass_constant_buffers(f32 width, f32 height);
      void update_object_constant_buffers(const rex::renderer::ResourceSlot& currObjectCommittedResource);
      void render_items(SceneRenderer* renderer, const rsl::vector<ResourceSlot>& contantBuffers) const;
      void use_shader();
      void use_pso();
      void update_view();

      rex::renderer::ResourceSlot get_object_committed_resource_of_frame(const rex::renderer::ResourceSlot* frame);
      rex::renderer::ResourceSlot get_pass_committed_resource_of_frame(const rex::renderer::ResourceSlot* frame);
      rex::renderer::ResourceSlot get_active_pass_constant_buffer_for_frame(s32 frame);
      rsl::vector<rex::renderer::ResourceSlot> get_active_constant_buffer_for_frame(s32 frame);

    private:
      RenderItems m_render_items;
      rex::renderer::ResourceSlot m_shader_program;
      rex::renderer::ResourceSlot m_input_layout;
      rex::renderer::ResourceSlot m_raster_state;
      rex::renderer::ResourceSlot m_pso;

      PassConstants m_pass_constants;
      rsl::vector<FrameData> m_frame_resource_data;

      // These should be part of a camera class
      glm::vec3 m_eye_pos = { 0.0f, 0.0f, 0.0f };
      glm::mat4 m_view = glm::mat4(1.0f);
      glm::mat4 m_proj = glm::mat4(1.0f);
    };
  } // namespace renderer
} // namespace rex