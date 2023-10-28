#pragma once

#include "rex_engine/types.h"

#include "rex_renderer_core/parameters/renderer_params.h"

#include "rex_renderer_core/viewport.h"
#include "rex_renderer_core/scissor_rect.h"
#include "rex_renderer_core/shader_type.h"
#include "rex_renderer_core/shader_platform.h"
#include "rex_renderer_core/primitive_topology.h"
#include "rex_renderer_core/index_buffer_format.h"

#include <rex_std/bonus/string/stack_string.h>
#include <rex_std/bonus/string.h>

namespace rex
{
  namespace renderer
  {
    struct Info
    {
        rsl::small_stack_string api_version;
        rsl::small_stack_string shader_version;
        rsl::small_stack_string adaptor;
        rsl::small_stack_string vendor;
    };

    struct OutputWindowUserData
    {
        void* primary_display_handle;

        s32 window_width;
        s32 window_height;

        s32 refresh_rate;

        bool windowed;
    };

    bool            initialize(const OutputWindowUserData& userData, u32 maxCommands);
    void            shutdown();

    // general accessors
    const Info&     info();
    ShaderPlatform  shader_platform();

    bool            is_y_up();
    bool            is_depth_0_to_1();

    // public-api will buffer all commands for dispatch on dedicated thread
    u32             create_clear_state(const parameters::ClearState& clearStateParams);
    u32             create_raster_state(const parameters::RasterState& rasterStateParams);
    u32             create_input_layout(const parameters::CreateInputLayout& createInputLayoutParams);
    u32             create_buffer(const parameters::CreateBuffer& createBufferParams);
    u32             load_shader(const parameters::LoadShader& loadShaderParams);
    u32             link_shader(const parameters::LinkShader& linkShaderParams);
    u32             compile_shader(const parameters::CompileShader& compileShaderParams);

    void            release_resource(u32 resourceTarget);

    void            clear(u32 clearStateTarget);

    void            renderer_draw(u32 vertexCount, u32 startVertex, PrimitiveTopology primitiveTopology);
    void            renderer_draw_indexed(u32 indexCount, u32 startIndex, u32 baseVertex, PrimitiveTopology primitiveTopology);
    void            renderer_draw_indexed_instanced(u32 instanceCount, u32 startInstance, u32 indexCount, u32 startIndex, u32 baseVertex, PrimitiveTopology primitiveTopology);
    void            renderer_draw_instanced(u32 vertexCount, u32 instanceCount, u32 startVertex, u32 startInstance, PrimitiveTopology topology);

    void            set_raster_state(u32 rasterStateTarget);
    void            set_render_targets(u32* colorTargets, u32 numColorTargets, u32 depthTarget, u32 arrayIndex);
    void            set_render_targets(u32 colorTarget, u32 depthTarget);
    void            set_viewport(const Viewport& vp);
    void            set_scissor_rect(const ScissorRect& sr);
    void            set_input_layout(u32 inputLayoutTarget);
    void            set_vertex_buffer(u32 vertexBufferTarget, u32 startSlot, u32 stride, u32 offset);
    void            set_vertex_buffers(u32* vertexBufferTargets, u32 numBuffers, u32 startSlot, const u32* strides, const u32* offsets);
    void            set_index_buffer(u32 indexBufferTarget, IndexBufferFormat format, u32 offset);
    void            set_shader(u32 shaderTarget, ShaderType shaderType);

    void            new_frame();
    void            end_frame();

    void            present();

    namespace backend
    {
      void*         get_device();
      void*         get_command_queue();
      void*         get_command_list();
      void*         get_command_allocator();

      s32           get_backbuffer_format();
      s32           get_depthstencil_format();

      bool          get_msaa_enabled();
      s32           get_msaa_quality();

      bool          create_pipeline_state_object(void* psoDescription);

      bool          flush_command_queue();

      // Platform specific implementation, implements these function
      bool          create_clear_state(const parameters::ClearState& cs, u32 resourceSlot);
      bool          create_raster_state(const parameters::RasterState& rs, u32 resourceSlot);
      bool          create_input_layout(const parameters::CreateInputLayout& cil, u32 resourceSlot);
      bool          create_buffer(const parameters::CreateBuffer& cb, u32 resourceSlot);

      bool          load_shader(const parameters::LoadShader& ls, u32 resourceSlot);
      bool          link_shader(const parameters::LinkShader& ls, u32 resourceSlot);
      bool          compile_shader(const parameters::CompileShader& cs, u32 resourceSlot);

      void          clear(u32 clearStateIndex);

      bool          release_resource(u32 resourceSlot);

      bool  	    initialize(const OutputWindowUserData& userData, u32 bbColorTargetSlot, u32 bbDepthTargetSlot);
      void          shutdown();

      void          draw(u32 vertexCount, u32 startVertex, PrimitiveTopology topology);
      void          draw_indexed(u32 indexCount, u32 startIndex, u32 baseVertex, PrimitiveTopology topology);
      void          draw_instanced(u32 vertexCount, u32 instanceCount, u32 startVertex, u32 startInstance, PrimitiveTopology topology);
      void          draw_indexed_instanced(u32 instanceCount, u32 startInstance, u32 indexCount, u32 startIndex, u32 baseVertex, PrimitiveTopology topology);

      bool          set_render_targets(const u32* const colorTargets, u32 numColorTargets, u32 depthTarget, u32 color_slice = 0, u32 depth_slice = 0);
      bool          set_input_layout(u32 inputLayoutSlot);
      bool          set_viewport(const Viewport& viewport);
      bool          set_scissor_rect(const ScissorRect& rect);
      bool          set_vertex_buffers(u32* bufferIndices, u32 numBuffers, u32 startSlot, const u32* strides, const u32* offsets);
      bool          set_index_buffer(u32 bufferIndex, IndexBufferFormat format, u32 offset);
      bool          set_shader(u32 shaderIndex, ShaderType shaderType);
      bool          set_raster_state(u32 rasterStateIndex);

      // functions to synchronize the render and main threads
      bool          new_frame();
      bool          end_frame();

      bool          present();
    } // namespace backend
  }   // namespace renderer
} // namespace rex