#include "rex_renderer_core/renderer.h"
#include "rex_renderer_core/commands/render_cmd.h"
#include "rex_renderer_core/resource_slots.h"
#include "rex_renderer_core/log.h"
#include "rex_renderer_core/shader_type.h"
#include "rex_renderer_core/commands/begin_draw_cmd.h"
#include "rex_renderer_core/commands/clear_cmd.h"
#include "rex_renderer_core/commands/compile_shader_cmd.h"
#include "rex_renderer_core/commands/create_buffer_cmd.h"
#include "rex_renderer_core/commands/create_clear_state_cmd.h"
#include "rex_renderer_core/commands/create_constant_buffer_cmd.h"
#include "rex_renderer_core/commands/create_index_buffer_cmd.h"
#include "rex_renderer_core/commands/create_input_layout_cmd.h"
#include "rex_renderer_core/commands/create_pipeline_state_cmd.h"
#include "rex_renderer_core/commands/create_raster_state_cmd.h"
#include "rex_renderer_core/commands/create_vertex_buffer_cmd.h"
#include "rex_renderer_core/commands/create_frame_resource_cmd.h"
#include "rex_renderer_core/commands/draw_cmd.h"
#include "rex_renderer_core/commands/draw_indexed_cmd.h"
#include "rex_renderer_core/commands/draw_indexed_instanced_cmd.h"
#include "rex_renderer_core/commands/draw_instanced_cmd.h"
#include "rex_renderer_core/commands/end_draw_cmd.h"
#include "rex_renderer_core/commands/link_shader_cmd.h"
#include "rex_renderer_core/commands/load_shader_cmd.h"
#include "rex_renderer_core/commands/new_frame_cmd.h"
#include "rex_renderer_core/commands/end_frame_cmd.h"
#include "rex_renderer_core/commands/present_cmd.h"
#include "rex_renderer_core/commands/prepare_user_initialization_cmd.h"
#include "rex_renderer_core/commands/finish_user_initialization_cmd.h"
#include "rex_renderer_core/commands/release_resource_cmd.h"
#include "rex_renderer_core/commands/set_constant_buffer_cmd.h"
#include "rex_renderer_core/commands/set_index_buffer_cmd.h"
#include "rex_renderer_core/commands/set_input_layout_cmd.h"
#include "rex_renderer_core/commands/set_pipeline_state_cmd.h"
#include "rex_renderer_core/commands/set_primitive_topology_cmd.h"
#include "rex_renderer_core/commands/set_raster_state_cmd.h"
#include "rex_renderer_core/commands/set_render_target_cmd.h"
#include "rex_renderer_core/commands/set_scissor_rect_cmd.h"
#include "rex_renderer_core/commands/set_shader_cmd.h"
#include "rex_renderer_core/commands/set_vertex_buffer_cmd.h"
#include "rex_renderer_core/commands/set_viewport_cmd.h"
#include "rex_renderer_core/commands/update_constant_buffer_cmd.h"

#include "rex_engine/defines.h"
#include "rex_engine/ring_buffer.h"

#include "rex_std_extra/utility/enum_reflection.h"

#if REX_SINGLE_THREADED
#define add_cmd(cmd) exec_cmd(cmd)
#else
#define add_cmd(cmd) g_ctx.cmd_buffer.put(rsl::move(cmd))
#endif

namespace rex
{
    namespace globals
    {
        DefaultDepthInfo g_default_depth_info; // NOLINT(fuchsia-statically-constructed-objects, cppcoreguidelines-avoid-non-const-global-variables)
        DefaultTargetsInfo g_default_targets_info; // NOLINT(fuchsia-statically-constructed-objects, cppcoreguidelines-avoid-non-const-global-variables)

        //-------------------------------------------------------------------------
        const DefaultDepthInfo& default_depth_info()
        {
            return g_default_depth_info;
        }

        //-------------------------------------------------------------------------
        const DefaultTargetsInfo& default_targets_info()
        {
            return g_default_targets_info;
        }

    } // namespace globals

    namespace renderer
    {
        struct Context
        {
            ResourceSlots slot_resources;

            RingBuffer<RenderCommand> cmd_buffer;
        };

        Context g_ctx;

        namespace internal
        {
            //-------------------------------------------------------------------------
            template<typename TCommandType>
            ResourceSlot create_buffer(commands::CreateBufferCommandDesc&& createBufferParams)
            {
                ResourceSlot resource_slot = g_ctx.slot_resources.next_slot();

                TCommandType cmd(rsl::move(createBufferParams), resource_slot);

                if (!add_cmd(cmd))
                {
                    REX_ERROR(LogRendererCore, "Unable to add/exec create buffer command");
                    return ResourceSlot::make_invalid();
                }

                return resource_slot;
            }

            //-------------------------------------------------------------------------
            ResourceSlot create_constant_buffer(commands::CreateConstantBufferCommandDesc&& createBufferParams)
            {
                ResourceSlot resource_slot = g_ctx.slot_resources.next_slot();

                commands::CreateConstantBuffer cmd(rsl::move(createBufferParams), resource_slot);

                if (!add_cmd(cmd))
                {
                    REX_ERROR(LogRendererCore, "Unable to add/exec create constant buffer command");
                    return ResourceSlot::make_invalid();
                }

                return resource_slot;
            }
        }

        //-------------------------------------------------------------------------
        bool initialize(const OutputWindowUserData& userData, s32 maxCommands, s32 maxFrameResources)
        {
            UNUSED_PARAM(userData);

            g_ctx.slot_resources.initialize(32);
            g_ctx.cmd_buffer.initialize(maxCommands);

            globals::g_default_targets_info.front_buffer_color = g_ctx.slot_resources.next_slot();
            globals::g_default_targets_info.back_buffer_color = g_ctx.slot_resources.next_slot();
            globals::g_default_targets_info.depth_buffer = g_ctx.slot_resources.next_slot();

            return backend::initialize(userData
                , maxFrameResources
                , globals::g_default_targets_info.front_buffer_color.slot_id()
                , globals::g_default_targets_info.back_buffer_color.slot_id()
                , globals::g_default_targets_info.depth_buffer.slot_id());
        }
        //-------------------------------------------------------------------------
        void shutdown()
        {
            backend::shutdown();
        }

        //-------------------------------------------------------------------------
        ResourceSlot create_clear_state(commands::CreateClearStateCommandDesc&& desc)
        {
            ResourceSlot resource_slot = g_ctx.slot_resources.next_slot();

            commands::CreateClearState cmd(rsl::move(desc), resource_slot);

            if (!add_cmd(cmd))
            {
                REX_ERROR(LogRendererCore, "Unable to add/exec create clear state command");
                return ResourceSlot::make_invalid();
            }

            return resource_slot;
        }

        //-------------------------------------------------------------------------
        ResourceSlot create_raster_state(commands::CreateRasterStateCommandDesc&& desc)
        {
            ResourceSlot resource_slot = g_ctx.slot_resources.next_slot();

            commands::CreateRasterState cmd(rsl::move(desc), resource_slot);

            if (!add_cmd(cmd))
            {
                REX_ERROR(LogRendererCore, "Unable to add/exec create raster state command");
                return ResourceSlot::make_invalid();
            }

            return resource_slot;
        }

        //-------------------------------------------------------------------------
        ResourceSlot create_input_layout(commands::CreateInputLayoutCommandDesc&& desc)
        {
            ResourceSlot resource_slot = g_ctx.slot_resources.next_slot();

            commands::CreateInputLayout cmd(rsl::move(desc), resource_slot);

            if (!add_cmd(cmd))
            {
                REX_ERROR(LogRendererCore, "Unable to add/exec create input layout command");
                return ResourceSlot::make_invalid();
            }

            return resource_slot;
        }

        //-------------------------------------------------------------------------
        ResourceSlot create_vertex_buffer(commands::CreateBufferCommandDesc&& createBufferParams)
        {
            return internal::create_buffer<commands::CreateVertexBuffer>(rsl::move(createBufferParams));
        }

        //-------------------------------------------------------------------------
        ResourceSlot create_index_buffer(commands::CreateBufferCommandDesc&& createBufferParams)
        {
            return internal::create_buffer<commands::CreateIndexBuffer>(rsl::move(createBufferParams));
        }

        //-------------------------------------------------------------------------
        ResourceSlot create_constant_buffer(commands::CreateConstantBufferCommandDesc&& createBufferParams)
        {
            return internal::create_constant_buffer(rsl::move(createBufferParams));
        }

        //-------------------------------------------------------------------------
        ResourceSlot create_pipeline_state_object(commands::CreatePipelineStateCommandDesc&& createPipelineStateParams)
        {
            ResourceSlot resource_slot = g_ctx.slot_resources.next_slot();

            commands::CreatePipelineState cmd(rsl::move(createPipelineStateParams), resource_slot);

            if (!add_cmd(cmd))
            {
                REX_ERROR(LogRendererCore, "Unable to add/exec create pipeline state object command");
                return ResourceSlot::make_invalid();
            }

            return resource_slot;
        }

        //-------------------------------------------------------------------------
        ResourceSlot create_frame_resource()
        {
            ResourceSlot resource_slot = g_ctx.slot_resources.next_slot();

            commands::CreateFrameResource cmd(commands::CreateFrameResourceCommandDesc(), resource_slot);

            if (!add_cmd(cmd))
            {
                REX_ERROR(LogRendererCore, "Unable to add/exec create frame resource command");
                return ResourceSlot::make_invalid();
            }

            return resource_slot;
        }

        //-------------------------------------------------------------------------
        ResourceSlot load_shader(commands::LoadShaderCommandDesc&& loadShaderParams)
        {
            ResourceSlot resource_slot = g_ctx.slot_resources.next_slot();

            commands::LoadShader cmd(rsl::move(loadShaderParams), resource_slot);

            if (!add_cmd(cmd))
            {
                REX_ERROR(LogRendererCore, "Unable to add/exec load shader command");
                return ResourceSlot::make_invalid();
            }

            return resource_slot;
        }

        //-------------------------------------------------------------------------
        ResourceSlot link_shader(commands::LinkShaderCommandDesc&& linkShaderParams)
        {
            ResourceSlot resource_slot = g_ctx.slot_resources.next_slot();

            commands::LinkShader cmd(rsl::move(linkShaderParams), resource_slot);

            if (!add_cmd(cmd))
            {
                REX_ERROR(LogRendererCore, "Unable to add/exec link shader command");
                return ResourceSlot::make_invalid();
            }

            return resource_slot;
        }

        //-------------------------------------------------------------------------
        ResourceSlot compile_shader(commands::CompileShaderCommandDesc&& compileShaderParams)
        {
            ResourceSlot resource_slot = g_ctx.slot_resources.next_slot();

            commands::CompileShader cmd(rsl::move(compileShaderParams), resource_slot);

            if (!add_cmd(cmd))
            {
                REX_ERROR(LogRendererCore, "Unable to add/exec compile shader command");
                return ResourceSlot::make_invalid();
            }

            return resource_slot;
        }

        //-------------------------------------------------------------------------
        bool update_constant_buffer(commands::UpdateConstantBufferCommandDesc&& updateConstantBufferParams, ResourceSlot constantBufferTarget)
        {
            commands::UpdateConstantBuffer cmd(rsl::move(updateConstantBufferParams), constantBufferTarget);

            return add_cmd(cmd);
        }

        //-------------------------------------------------------------------------
        void wait_for_active_frame()
        {
            backend::wait_for_active_frame();
        }

        //-------------------------------------------------------------------------
        bool release_resource(ResourceSlot resourceTarget)
        {
            commands::ReleaseResource cmd(commands::ReleaseResourceCommandDesc {&g_ctx.slot_resources}, resourceTarget);

            return add_cmd(cmd);
        }

        //-------------------------------------------------------------------------
        bool prepare_user_initialization()
        {
            commands::PrepareUserInitialization cmd(commands::PrepareUserInitializationCommandDesc {});

            return add_cmd(cmd);
        }

        //-------------------------------------------------------------------------
        bool finish_user_initialization()
        {
            commands::FinishUserInitialization cmd(commands::FinishUserInitializationCommandDesc {});

            return add_cmd(cmd);
        }

        //-------------------------------------------------------------------------
        bool clear(ResourceSlot clearStateTarget)
        {
            commands::Clear cmd(commands::ClearCommandDesc(), clearStateTarget);

            return add_cmd(cmd);
        }

        //-------------------------------------------------------------------------
        bool renderer_draw(s32 vertexCount, s32 startVertex)
        {
            commands::Draw cmd(commands::DrawCommandDesc {vertexCount, startVertex});

            return add_cmd(cmd);
        }

        //-------------------------------------------------------------------------
        bool renderer_draw_indexed(s32 indexCount, s32 startIndex, s32 baseVertex)
        {
            commands::DrawIndexed cmd(commands::DrawIndexedCommandDesc {indexCount, startIndex, baseVertex});

            return add_cmd(cmd);
        }

        //-------------------------------------------------------------------------
        bool renderer_draw_indexed_instanced(s32 instanceCount, s32 startInstance, s32 indexCount, s32 startIndex, s32 baseVertex)
        {
            commands::DrawIndexedInstanced cmd(commands::DrawIndexedInstancedCommandDesc {instanceCount, startInstance, indexCount, startIndex, baseVertex });

            return add_cmd(cmd);
        }

        //-------------------------------------------------------------------------
        bool renderer_draw_instanced(s32 vertexCount, s32 instanceCount, s32 startVertex, s32 startInstance)
        {
            commands::DrawInstanced cmd(commands::DrawInstanceCommandDesc {instanceCount, startInstance, vertexCount, startVertex});

            return add_cmd(cmd);
        }

        //-------------------------------------------------------------------------
        bool set_raster_state(ResourceSlot rasterStateTarget)
        {
            commands::SetRasterState cmd(commands::SetRasterStateCommandDesc {}, rasterStateTarget);

            return add_cmd(cmd);
        }

        //-------------------------------------------------------------------------
        bool set_render_targets(ResourceSlot* colorTargets, s32 numColorTargets, ResourceSlot depthTarget)
        {
            commands::SetRenderTargetCommandDesc set_render_target_command_desc;
            set_render_target_command_desc.num_color = numColorTargets;
            memcpy(&set_render_target_command_desc.color, colorTargets, numColorTargets * sizeof(ResourceSlot));
            set_render_target_command_desc.depth = depthTarget;

            commands::SetRenderTarget cmd(rsl::move(set_render_target_command_desc));

            return add_cmd(cmd);
        }

        //-------------------------------------------------------------------------
        bool set_render_targets(ResourceSlot colorTarget, ResourceSlot depthTarget)
        {
            commands::SetRenderTargetCommandDesc set_render_target_command_desc;
            set_render_target_command_desc.num_color = colorTarget.is_valid() ? 1 : 0;
            set_render_target_command_desc.color[0] = colorTarget;
            set_render_target_command_desc.depth = depthTarget;

            commands::SetRenderTarget cmd(rsl::move(set_render_target_command_desc));

            return add_cmd(cmd);
        }

        //-------------------------------------------------------------------------
        bool set_viewport(const Viewport& vp)
        {
            commands::SetViewport cmd(commands::SetViewportCommandDesc {vp});

            return add_cmd(cmd);
        }

        //-------------------------------------------------------------------------
        bool set_scissor_rect(const ScissorRect& sr)
        {
            commands::SetScissorRect cmd(commands::SetScissorRectCommandDesc {sr});

            return add_cmd(cmd);
        }

        //-------------------------------------------------------------------------
        bool set_input_layout(ResourceSlot inputLayoutTarget)
        {
            commands::SetInputLayout cmd(commands::SetInputLayoutCommandDesc {}, inputLayoutTarget);

            return add_cmd(cmd);
        }

        //-------------------------------------------------------------------------
        bool set_vertex_buffer(ResourceSlot vertexBufferTarget, s32 startSlot, s32 stride, s32 offset)
        {
            ResourceSlot* vertex_buffer_target = &vertexBufferTarget;
            const s32* stride_target = &stride;
            const s32* offset_target = &offset;

            return set_vertex_buffers(vertex_buffer_target, 1, startSlot, stride_target, offset_target);
        }
        //-------------------------------------------------------------------------
        bool set_vertex_buffers(ResourceSlot* vertexBufferTargets, s32 numBuffers, s32 startSlot, const s32* strides, const s32* offsets)
        {
            commands::SetVertexBufferCommandDesc set_vertex_buffer_command_desc;

            set_vertex_buffer_command_desc.start_slot = startSlot;

            set_vertex_buffer_command_desc.vertex_buffer_targets = rsl::vector<ResourceSlot>((rsl::Size)numBuffers);
            set_vertex_buffer_command_desc.strides = rsl::vector<s32>((rsl::Size)numBuffers);
            set_vertex_buffer_command_desc.offsets = rsl::vector<s32>((rsl::Size)numBuffers);

            for (s32 i = 0; i < numBuffers; ++i)
            {
                set_vertex_buffer_command_desc.vertex_buffer_targets[i] = vertexBufferTargets[i];
                set_vertex_buffer_command_desc.strides[i] = strides[i];
                set_vertex_buffer_command_desc.offsets[i] = offsets[i];
            }

            commands::SetVertexBuffer cmd(rsl::move(set_vertex_buffer_command_desc));

            return add_cmd(cmd);
        }
        //-------------------------------------------------------------------------
        bool set_index_buffer(ResourceSlot indexBufferTarget, IndexBufferFormat format, s32 offset)
        {
            commands::SetIndexBuffer cmd(commands::SetIndexBufferCommandDesc {format, offset}, indexBufferTarget);

            return add_cmd(cmd);
        }

        //-------------------------------------------------------------------------
        bool set_shader(ResourceSlot shaderTarget)
        {
            commands::SetShader cmd(commands::SetShaderCommandDesc {}, shaderTarget);

            return add_cmd(cmd);
        }

        //-------------------------------------------------------------------------
        bool set_pipeline_state_object(ResourceSlot psoTarget)
        {
            commands::SetPipelineState cmd(commands::SetPipelineStateCommandDesc {}, psoTarget);

            return add_cmd(cmd);
        }

        //-------------------------------------------------------------------------
        bool set_constant_buffer(ResourceSlot constantBufferTarget, s32 location)
        {
            commands::SetConstantBuffer cmd(commands::SetConstantBufferCommandDesc {location}, constantBufferTarget);

            return add_cmd(cmd);
        }

        //-------------------------------------------------------------------------
        bool set_primitive_topology(PrimitiveTopology primitiveTopology)
        {
            commands::SetPrimitiveTopology cmd(commands::SetPrimitiveTopologyCommandDesc {primitiveTopology});

            return add_cmd(cmd);
        }

        //-------------------------------------------------------------------------
        bool new_frame()
        {
            commands::NewFrame cmd(commands::NewFrameCommandDesc {});

            return add_cmd(cmd);
        }

        //-------------------------------------------------------------------------
        bool end_frame()
        {
            commands::EndFrame cmd(commands::EndFrameCommandDesc {});

            return add_cmd(cmd);
        }

        //-------------------------------------------------------------------------
        bool begin_draw()
        {
            commands::BeginDraw cmd(commands::BeginDrawCommandDesc {});

            return add_cmd(cmd);
        }

        //-------------------------------------------------------------------------
        bool end_draw()
        {
            commands::EndDraw cmd(commands::EndDrawCommandDesc {});

            return add_cmd(cmd);
        }

        //-------------------------------------------------------------------------
        bool present()
        {
            commands::Present cmd(commands::PresentCommandDesc {&globals::g_default_targets_info.front_buffer_color, &globals::g_default_targets_info.back_buffer_color});

            return add_cmd(cmd);
        }

        //-------------------------------------------------------------------------
        bool flush()
        {
            RenderCommand* cmd = g_ctx.cmd_buffer.get();

            while (cmd)
            {
                if (cmd->execute() == false)
                {
                    REX_ERROR(LogRendererCore, "Failed to flush commands");
                    return false;
                }

                cmd = g_ctx.cmd_buffer.get();
            }

            return true;
        }
    } // namespace renderer
} // namespace rex