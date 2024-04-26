#pragma once

#include "rex_renderer_core/rendering/render_item.h"
#include "rex_renderer_core/rendering/fill_mode.h"
#include "rex_std/vector.h"

#include "rex_renderer_core/resource_management/resource.h"

namespace rex
{
  namespace renderer
  {
    // A scene is the renderer's representation of the world.
    // It should not hold any data that's used for logic and only hold data that's relevant
    // for drawing objects that are in the world.
    class Scene
    {
    public:
      using RenderItems       = rsl::vector<RenderItem>;
      using RenderItemIt      = RenderItems::iterator;
      using ConstRenderItemIt = RenderItems::const_iterator;

      // Construct a default scene with no objects
      Scene();
      virtual ~Scene() = default;

      // Update the render data of the scene.
      // This is where data upload and download to/from the GPU occurs.
      void update();

    protected:
      // At the moment we have hardcoded scenes with hardcoded shaders etc.
      // In the future the below function will be removed from the scene.
      // The renderer should be responsible for these objects.

      // Initialization
      void build_shader(rsl::string_view vertexShaderPath, rsl::string_view pixelShaderPath);
      void build_input_layout();
      void build_raster_state(FillMode fillMode = FillMode::Solid);
      void build_pso();

      // Update
      void use_shader();
      void use_pso();

      virtual void update_object_constant_buffers() = 0;

    private:
      rex::rhi::ResourceSlot m_shader_program;
      rex::rhi::ResourceSlot m_input_layout;
      rex::rhi::ResourceSlot m_raster_state;
      rex::rhi::ResourceSlot m_pso;
    };
  } // namespace renderer
} // namespace rex