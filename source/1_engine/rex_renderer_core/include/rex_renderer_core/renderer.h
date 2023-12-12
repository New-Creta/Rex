#pragma once

#include "rex_engine/types.h"

#include "rex_renderer_core/viewport.h"
#include "rex_renderer_core/scissor_rect.h"
#include "rex_renderer_core/shader_platform.h"
#include "rex_renderer_core/primitive_topology.h"
#include "rex_renderer_core/index_buffer_format.h"
#include "rex_renderer_core/default_depth_info.h"
#include "rex_renderer_core/default_targets_info.h"

#include <rex_std/bonus/string/stack_string.h>
#include <rex_std/bonus/string.h>

namespace rex
{
    namespace globals
    {
        const DefaultTargetsInfo& default_targets_info();
        const DefaultDepthInfo& default_depth_info();
    }

    namespace renderer
    {
        namespace commands
        {
            class CreateClearStateCommandDesc;
            class CreateRasterStateCommandDesc;
            class CreateBufferCommandDesc;
            class CreateIndexBufferCommandDesc;
            class CreateConstantBufferCommandDesc;
            class CreatePipelineStateCommandDesc;
            class CreateInputLayoutCommandDesc;

            class BeginDrawCommandDesc;
            class EndDrawCommandDesc;
            class NewFrame;
            class EndFrameCommandDesc;

            class CompileShaderCommandDesc;
            class LinkShaderCommandDesc;
            class LoadShaderCommandDesc;

            class ClearCommandDesc;
            class DrawCommandDesc;
            class DrawIndexedCommandDesc;
            class DrawIndexedInstancedCommandDesc;
            class DrawInstanceCommandDesc;
            class ReleaseResourceCommandDesc;

            class UpdateConstantBufferCommandDesc;
        }

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

        bool            initialize(const OutputWindowUserData& userData, s32 maxCommands, s32 maxFrameResources);
        void            shutdown();

        // general accessors
        const Info&     info();
        ShaderPlatform  shader_platform();

        bool            is_y_up();
        bool            is_depth_0_to_1();

        // public-api will buffer all commands for dispatch on dedicated thread
        s32             active_frame();
        s32             num_frames_in_flight();

        ResourceSlot    create_clear_state(commands::CreateClearStateCommandDesc&& clearStateParams);
        ResourceSlot    create_raster_state(commands::CreateRasterStateCommandDesc&& rasterStateParams);
        ResourceSlot    create_input_layout(commands::CreateInputLayoutCommandDesc&& createInputLayoutParams);
        ResourceSlot    create_vertex_buffer(commands::CreateBufferCommandDesc&& createBufferParams);
        ResourceSlot    create_index_buffer(commands::CreateBufferCommandDesc&& createBufferParams);
        ResourceSlot    create_constant_buffer(commands::CreateConstantBufferCommandDesc&& createBufferParams);
        ResourceSlot    create_pipeline_state_object(commands::CreatePipelineStateCommandDesc&& createPipelineStateParams);
        ResourceSlot    create_frame_resource();

        ResourceSlot    load_shader(commands::LoadShaderCommandDesc&& loadShaderParams);
        ResourceSlot    link_shader(commands::LinkShaderCommandDesc&& linkShaderParams);
        ResourceSlot    compile_shader(commands::CompileShaderCommandDesc&& compileShaderParams);

        bool            update_constant_buffer(commands::UpdateConstantBufferCommandDesc&& updateConstantBufferParams, ResourceSlot constantBufferTarget);

        void            wait_for_active_frame();

        bool            release_resource(ResourceSlot resourceTarget);

        bool            prepare_user_initialization();
        bool            finish_user_initialization();

        bool            clear(ResourceSlot clearStateTarget);

        bool            renderer_draw(s32 vertexCount, s32 startVertex);
        bool            renderer_draw_indexed(s32 indexCount, s32 startIndex, s32 baseVertex);
        bool            renderer_draw_indexed_instanced(s32 instanceCount, s32 startInstance, s32 indexCount, s32 startIndex, s32 baseVertex);
        bool            renderer_draw_instanced(s32 vertexCount, s32 instanceCount, s32 startVertex, s32 startInstance);

        bool            set_raster_state(ResourceSlot rasterStateTarget);
        bool            set_render_targets(ResourceSlot* colorTargets, s32 numColorTargets, ResourceSlot depthTarget);
        bool            set_render_targets(ResourceSlot colorTarget, ResourceSlot depthTarget);
        bool            set_viewport(const Viewport& vp);
        bool            set_scissor_rect(const ScissorRect& sr);
        bool            set_input_layout(ResourceSlot inputLayoutTarget);
        bool            set_vertex_buffer(ResourceSlot vertexBufferTarget, s32 startSlot, s32 stride, s32 offset);
        bool            set_vertex_buffers(ResourceSlot* vertexBufferTargets, s32 numBuffers, s32 startSlot, const s32* strides, const s32* offsets);
        bool            set_index_buffer(ResourceSlot indexBufferTarget, IndexBufferFormat format, s32 offset);
        bool            set_shader(ResourceSlot shaderTarget);
        bool            set_pipeline_state_object(ResourceSlot psoTarget);
        bool            set_constant_buffer(ResourceSlot constantBufferTarget, s32 location);
        bool            set_primitive_topology(PrimitiveTopology primitiveTopology);

        bool            new_frame();
        bool            end_frame();

        bool            begin_draw();
        bool            end_draw();

        bool            present();
        bool            flush();

        namespace backend
        {
            bool          flush_command_queue();

            bool  	      initialize(const OutputWindowUserData& userData, s32 maxFrameResources, s32 fbColorTargetSlot, s32 bbColorTargetSlot, s32 depthTargetSlot);
            void          shutdown();

            // Platform specific implementation, implements these function
            bool          create_clear_state(const commands::CreateClearStateCommandDesc& cs, ResourceSlot resourceSlot);
            bool          create_raster_state(const commands::CreateRasterStateCommandDesc& rs, ResourceSlot resourceSlot);
            bool          create_input_layout(const commands::CreateInputLayoutCommandDesc& cil, ResourceSlot resourceSlot);
            bool          create_vertex_buffer(const commands::CreateBufferCommandDesc& cb, ResourceSlot resourceSlot);
            bool          create_index_buffer(const commands::CreateBufferCommandDesc& cb, ResourceSlot resourceSlot);
            bool          create_constant_buffer(const commands::CreateConstantBufferCommandDesc& cb, ResourceSlot resourceSlot);
            bool          create_pipeline_state_object(const commands::CreatePipelineStateCommandDesc& cps, ResourceSlot resourceSlot);
            bool          create_frame_resource(ResourceSlot resourceSlot);

            bool          load_shader(const commands::LoadShaderCommandDesc& ls, ResourceSlot resourceSlot);
            bool          link_shader(const commands::LinkShaderCommandDesc& ls, ResourceSlot resourceSlot);
            bool          compile_shader(const commands::CompileShaderCommandDesc& cs, ResourceSlot resourceSlot);

            void          update_constant_buffer(const commands::UpdateConstantBufferCommandDesc& updateConstantBuffer, ResourceSlot resourceSlot);
            
            void          wait_for_active_frame();

            bool          release_resource(ResourceSlot resourceSlot);

            bool          prepare_user_initialization();
            bool          finish_user_initialization();

            void          clear(ResourceSlot clearStateIndex);

            void          draw(s32 vertexCount, s32 startVertex);
            void          draw_indexed(s32 indexCount, s32 startIndex, s32 baseVertex);
            void          draw_instanced(s32 vertexCount, s32 instanceCount, s32 startVertex, s32 startInstance);
            void          draw_indexed_instanced(s32 instanceCount, s32 startInstance, s32 indexCount, s32 startIndex, s32 baseVertex);

            bool          set_render_targets(const ResourceSlot* const colorTargets, s32 numColorTargets, ResourceSlot depthTarget);
            bool          set_input_layout(ResourceSlot inputLayoutTarget);
            bool          set_viewport(const Viewport& viewport);
            bool          set_scissor_rect(const ScissorRect& rect);
            bool          set_vertex_buffers(ResourceSlot* vertexBufferTargets, s32 numBuffers, s32 startSlot, const s32* strides, const s32* offsets);
            bool          set_index_buffer(ResourceSlot indexBufferTarget, IndexBufferFormat format, s32 offset);
            bool          set_constant_buffer(ResourceSlot constantBufferTarget, s32 location);
            bool          set_shader(ResourceSlot shaderTarget);
            bool          set_raster_state(ResourceSlot rasterStateTarget);
            bool          set_pipeline_state_object(ResourceSlot psoTarget);
            bool          set_primitive_topology(PrimitiveTopology primitiveTopology);

            // functions to synchronize the render and main threads
            bool          new_frame();
            bool          end_frame();

            bool          begin_draw();
            bool          end_draw();

            bool          present();
        }       // namespace backend
    }     // namespace renderer
}   // namespace rex