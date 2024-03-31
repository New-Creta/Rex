#pragma once

#include "rex_renderer_core/rendering/render_item.h"
#include "rex_renderer_core/rendering/fill_mode.h"
#include "rex_std/vector.h"

#include "rex_renderer_core/resource_management/resource.h"

namespace rex
{
  namespace renderer
  {
    class Scene
    {
    public:
      using RenderItems       = rsl::vector<RenderItem>;
      using RenderItemIt      = RenderItems::iterator;
      using ConstRenderItemIt = RenderItems::const_iterator;

      Scene();
      virtual ~Scene() = default;

      void update();

    protected:
      // Initialization
      void build_shader(rsl::string_view vertexShaderPath, rsl::string_view pixelShaderPath);
      void build_input_layout();
      void build_raster_state(FillMode fillMode = FillMode::SOLID);
      void build_pso();

      // Update
      void use_shader();
      void use_pso();
      void update_view();

      virtual void update_object_constant_buffers() = 0;

    private:
      rex::rhi::ResourceSlot m_shader_program;
      rex::rhi::ResourceSlot m_input_layout;
      rex::rhi::ResourceSlot m_raster_state;
      rex::rhi::ResourceSlot m_pso;

      // These should be part of a camera class
      glm::vec3 m_eye_pos = { 0.0f, 0.0f, 0.0f };
      glm::mat4 m_view = glm::mat4(1.0f);
      glm::mat4 m_proj = glm::mat4(1.0f);
    };
  } // namespace renderer
} // namespace rex