#pragma once

#include "rex_renderer_core/rendering/primitive_topology.h"
#include "rex_renderer_core/rendering/renderer_output_window_user_data.h"
#include "rex_renderer_core/resource_management/resource_slot.h"
#include "rex_renderer_core/resources/clear_state.h"
#include "rex_renderer_core/resources/compile_shader.h"
#include "rex_renderer_core/resources/constant_buffer.h"
#include "rex_renderer_core/resources/index_buffer.h"
#include "rex_renderer_core/resources/input_layout.h"
#include "rex_renderer_core/resources/link_shader.h"
#include "rex_renderer_core/resources/mesh.h"
#include "rex_renderer_core/resources/pipeline_state.h"
#include "rex_renderer_core/resources/raster_state.h"
#include "rex_renderer_core/resources/shader.h"
#include "rex_renderer_core/resources/vertex_buffer.h"

namespace rex
{
  namespace rhi
  {
    // Some info about the rendering infrastructure
    struct Info
    {
      rsl::small_stack_string api_version;
      rsl::small_stack_string shader_version;
      rsl::small_stack_string adaptor;
      rsl::small_stack_string vendor;
      rsl::small_stack_string driver_version;
    };

    // Return basic info about the graphics hardware of the current machine
    const Info& info();

    // Initializes the render hardware infrastructure
    // For DirectX, creates the dxgi factory, d3d device, command buffers, heaps and swapchain
    // After this, the rhi is setup to start creating resources (textures, shaders, vertex buffers, ..)
    bool init(const renderer::OutputWindowUserData& userData);

    // shutdown the internal rhi, all reference to the rhi are invalidated from here on out
    void shutdown();
    
  } // namespace rhi
} // namespace rex