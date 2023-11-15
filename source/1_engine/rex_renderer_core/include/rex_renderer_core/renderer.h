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
    enum class DefaultTargets
    {
      REX_FRONT_BUFFER_COLOR = 1,
      REX_BACK_BUFFER_COLOR = 2,
      REX_BUFFER_DEPTH = 3
    };

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

    bool            initialize(const OutputWindowUserData& userData, s32 maxCommands);
    void            shutdown();

    // general accessors
    const Info&     info();
    ShaderPlatform  shader_platform();

    bool            is_y_up();
    bool            is_depth_0_to_1();

    // public-api will buffer all commands for dispatch on dedicated thread
    s32             create_clear_state(const parameters::ClearState& clearStateParams);
    s32             create_raster_state(const parameters::RasterState& rasterStateParams);
    s32             create_input_layout(const parameters::CreateInputLayout& createInputLayoutParams);
    s32             create_vertex_buffer(const parameters::CreateBuffer& createBufferParams);
    s32             create_index_buffer(const parameters::CreateBuffer& createBufferParams);
    s32             create_constant_buffer(const parameters::CreateConstantBuffer& createBufferParams);
    s32             create_pipeline_state_object(const parameters::CreatePipelineState& createPipelineStateParams);
    s32             load_shader(const parameters::LoadShader& loadShaderParams);
    s32             link_shader(const parameters::LinkShader& linkShaderParams);
    s32             compile_shader(const parameters::CompileShader& compileShaderParams);

    void            release_resource(s32 resourceTarget);

    void            clear(s32 clearStateTarget);

    void            renderer_draw(s32 vertexCount, s32 startVertex, PrimitiveTopology primitiveTopology);
    void            renderer_draw_indexed(s32 indexCount, s32 startIndex, s32 baseVertex, PrimitiveTopology primitiveTopology);
    void            renderer_draw_indexed_instanced(s32 instanceCount, s32 startInstance, s32 indexCount, s32 startIndex, s32 baseVertex, PrimitiveTopology primitiveTopology);
    void            renderer_draw_instanced(s32 vertexCount, s32 instanceCount, s32 startVertex, s32 startInstance, PrimitiveTopology topology);

    void            set_raster_state(s32 rasterStateTarget);
    void            set_render_targets(s32* colorTargets, s32 numColorTargets, s32 depthTarget);
    void            set_render_targets(s32 colorTarget, s32 depthTarget);
    void            set_viewport(const Viewport& vp);
    void            set_scissor_rect(const ScissorRect& sr);
    void            set_input_layout(s32 inputLayoutTarget);
    void            set_vertex_buffer(s32 vertexBufferTarget, s32 startSlot, s32 stride, s32 offset);
    void            set_vertex_buffers(s32* vertexBufferTargets, s32 numBuffers, s32 startSlot, const s32* strides, const s32* offsets);
    void            set_index_buffer(s32 indexBufferTarget, IndexBufferFormat format, s32 offset);
    void            set_shader(s32 shaderTarget, ShaderType shaderType);
    void            set_pipeline_state_object(s32 psoTarget);
    void            set_constant_buffer(s32 constantBufferTarget, s32 offset);

    void            new_frame();
    void            end_frame();

    bool            begin_draw();
    bool            end_draw();

    void            present();

    namespace backend
    {
      bool          flush_command_queue();

      // Platform specific implementation, implements these function
      bool          create_clear_state(const parameters::ClearState& cs, s32 resourceSlot);
      bool          create_raster_state(const parameters::RasterState& rs, s32 resourceSlot);
      bool          create_input_layout(const parameters::CreateInputLayout& cil, s32 resourceSlot);
      bool          create_vertex_buffer(const parameters::CreateBuffer& cb, s32 resourceSlot);
      bool          create_index_buffer(const parameters::CreateBuffer& cb, s32 resourceSlot);
      bool          create_constant_buffer(const parameters::CreateConstantBuffer& cb, s32 resourceSlot);
      bool          create_pipeline_state_object(const parameters::CreatePipelineState& cps, s32 resourceSlot);

      bool          load_shader(const parameters::LoadShader& ls, s32 resourceSlot);
      bool          link_shader(const parameters::LinkShader& ls, s32 resourceSlot);
      bool          compile_shader(const parameters::CompileShader& cs, s32 resourceSlot);

      void          clear(s32 clearStateIndex);

      bool          release_resource(s32 resourceSlot);

      bool  	    initialize(const OutputWindowUserData& userData, s32 fbColorTargetSlot, s32 bbColorTargetSlot, s32 depthTargetSlot);
      void          shutdown();

      void          draw(s32 vertexCount, s32 startVertex, PrimitiveTopology topology);
      void          draw_indexed(s32 indexCount, s32 startIndex, s32 baseVertex, PrimitiveTopology topology);
      void          draw_instanced(s32 vertexCount, s32 instanceCount, s32 startVertex, s32 startInstance, PrimitiveTopology topology);
      void          draw_indexed_instanced(s32 instanceCount, s32 startInstance, s32 indexCount, s32 startIndex, s32 baseVertex, PrimitiveTopology topology);

      bool          set_render_targets(const s32* const colorTargets, s32 numColorTargets, s32 depthTarget);
      bool          set_input_layout(s32 inputLayoutSlot);
      bool          set_viewport(const Viewport& viewport);
      bool          set_scissor_rect(const ScissorRect& rect);
      bool          set_vertex_buffers(s32* bufferIndices, s32 numBuffers, s32 startSlot, const s32* strides, const s32* offsets);
      bool          set_index_buffer(s32 bufferIndex, IndexBufferFormat format, s32 offset);
      bool          set_constant_buffer(s32 bufferIndex, s32 location, s32 offset);
      bool          set_shader(s32 shaderIndex, ShaderType shaderType);
      bool          set_raster_state(s32 rasterStateIndex);
      bool          set_pipeline_state_object(s32 psoTarget);

      // functions to synchronize the render and main threads
      bool          new_frame();
      bool          end_frame();

      bool          begin_draw();
      bool          end_draw();

      bool          present();
    }       // namespace backend
  }     // namespace renderer
}   // namespace rex