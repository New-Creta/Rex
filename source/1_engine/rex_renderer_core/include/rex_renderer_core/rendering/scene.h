#pragma once

#include "rex_renderer_core/rendering/render_item.h"
#include "rex_renderer_core/rendering/fill_mode.h"
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

    class FrameData
    {
    public:
      //-------------------------------------------------------------------------
      explicit FrameData(const rex::renderer::ResourceSlot& frameSlot)
        : m_frame_slot(frameSlot)
        , m_object_committed_resource_slot()
        , m_pass_committed_resource_slot()
        , m_object_constant_buffer_slots()
        , m_pass_constant_buffer_slot()
      {
      }

      //-------------------------------------------------------------------------
      FrameData(const FrameData& other) = delete;
      //-------------------------------------------------------------------------
      FrameData(FrameData&& other) noexcept = default;

      //-------------------------------------------------------------------------
      ~FrameData()
      {
        m_frame_slot.release();
        m_object_constant_buffer_slots.clear();
        m_pass_constant_buffer_slot.release();
      }

      //-------------------------------------------------------------------------
      FrameData& operator=(const FrameData& other) = delete;
      //-------------------------------------------------------------------------
      FrameData& operator=(FrameData&& other) noexcept = default;

      //-------------------------------------------------------------------------
      void set_object_committed_resource_slot(s32 frameIndex, s32 numItems, s32 bufferByteSize)
      {
        rex::renderer::commands::AttachCommittedResourceToFrameCommandDesc attach_object_constants{};
        attach_object_constants.frame_index = frameIndex;
        attach_object_constants.buffer_count = numItems;
        attach_object_constants.buffer_byte_size = bufferByteSize;

        m_object_committed_resource_slot = rex::renderer::attach_committed_resource_to_frame(rsl::move(attach_object_constants));
      }

      //-------------------------------------------------------------------------
      void set_pass_committed_resource_slot(s32 frameIndex, s32 numItems, s32 bufferByteSize)
      {
        rex::renderer::commands::AttachCommittedResourceToFrameCommandDesc attach_object_constants{};
        attach_object_constants.frame_index = frameIndex;
        attach_object_constants.buffer_count = numItems;
        attach_object_constants.buffer_byte_size = bufferByteSize;

        m_pass_committed_resource_slot = rex::renderer::attach_committed_resource_to_frame(rsl::move(attach_object_constants));
      }

      //-------------------------------------------------------------------------
      void add_object_constant_buffer_slot(s32 frameIndex, const rex::renderer::ResourceSlot& committedResourceSlot, s32 bufferSize)
      {
        rex::renderer::commands::CreateConstantBufferViewCommandDesc create_const_buffer_command_desc{};

        create_const_buffer_command_desc.frame_index = frameIndex;
        create_const_buffer_command_desc.committed_resource = committedResourceSlot;
        create_const_buffer_command_desc.buffer_size = bufferSize;

        rex::renderer::ResourceSlot const object_constant_buffer = rex::renderer::create_constant_buffer_view(rsl::move(create_const_buffer_command_desc));

        m_object_constant_buffer_slots.push_back(object_constant_buffer);
      }

      //-------------------------------------------------------------------------
      void set_pass_constant_buffer_slot(s32 frameIndex, const rex::renderer::ResourceSlot& committedResourceSlot, s32 bufferSize)
      {
        rex::renderer::commands::CreateConstantBufferViewCommandDesc create_const_buffer_command_desc{};

        create_const_buffer_command_desc.frame_index = frameIndex;
        create_const_buffer_command_desc.committed_resource = committedResourceSlot;
        create_const_buffer_command_desc.buffer_size = bufferSize;

        rex::renderer::ResourceSlot pass_constant_buffer = rex::renderer::create_constant_buffer_view(rsl::move(create_const_buffer_command_desc));

        m_pass_constant_buffer_slot = rsl::move(pass_constant_buffer);
      }

      //-------------------------------------------------------------------------
      const rex::renderer::ResourceSlot& frame_slot() const
      {
        return m_frame_slot;
      }

      //-------------------------------------------------------------------------
      const rex::renderer::ResourceSlot& object_committed_resource_slot() const
      {
        return m_object_committed_resource_slot;
      }

      //-------------------------------------------------------------------------
      const rex::renderer::ResourceSlot& pass_committed_resource_slot() const
      {
        return m_pass_committed_resource_slot;
      }

      //-------------------------------------------------------------------------
      const rsl::vector<rex::renderer::ResourceSlot>& object_constant_buffer_slots() const
      {
        return m_object_constant_buffer_slots;
      }

      //-------------------------------------------------------------------------
      const rex::renderer::ResourceSlot& pass_constant_buffer_slot() const
      {
        return m_pass_constant_buffer_slot;
      }

    private:
      rex::renderer::ResourceSlot m_frame_slot;
      rex::renderer::ResourceSlot m_object_committed_resource_slot;
      rex::renderer::ResourceSlot m_pass_committed_resource_slot;

      rsl::vector<rex::renderer::ResourceSlot> m_object_constant_buffer_slots;
      rex::renderer::ResourceSlot m_pass_constant_buffer_slot;
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