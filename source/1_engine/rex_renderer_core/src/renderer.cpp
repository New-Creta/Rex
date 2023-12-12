#include "rex_renderer_core/renderer.h"
#include "rex_renderer_core/commands/render_cmd.h"
#include "rex_renderer_core/resource_slots.h"
#include "rex_renderer_core/log.h"
#include "rex_renderer_core/commands/render_cmd.h"
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
        DefaultTargetsInfo g_default_targets_info; // NOLINT(fuchsia-statically-constructed-objects, cppcoreguidelines-avoid-non-const-global-variables)
        DefaultDepthInfo g_default_depth_info; // NOLINT(fuchsia-statically-constructed-objects, cppcoreguidelines-avoid-non-const-global-variables)

        //-------------------------------------------------------------------------
        const DefaultTargetsInfo& default_targets_info()
        {
            return g_default_targets_info;
        }
        //-------------------------------------------------------------------------
        const DefaultDepthInfo& default_depth_info()
        {
            return g_default_depth_info;
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
            ResourceSlot create_buffer(CommandType commandType, const commands::CreateBuffer& createBufferParams)
            {
                RenderCommand cmd;

                ResourceSlot resource_slot = g_ctx.slot_resources.next_slot();

                cmd.command_type = commandType;
                cmd.resource_slot = resource_slot.slot_id();

                memcpy(&cmd.create_buffer_params, (void*)&createBufferParams, sizeof(commands::CreateBuffer));

                if (createBufferParams.data)
                {
                    // make a copy of the buffers data
                    cmd.create_buffer_params.data = memory_alloc(createBufferParams.buffer_size);
                    memcpy(cmd.create_buffer_params.data, createBufferParams.data, createBufferParams.buffer_size);
                }

                if (!add_cmd(cmd))
                {
                    REX_ERROR(LogRendererCore, "Unable to add/exec create buffer command");
                    return ResourceSlot::make_invalid();
                }

                return resource_slot;
            }

            //-------------------------------------------------------------------------
            ResourceSlot create_constant_buffer(CommandType commandType, const commands::CreateConstantBuffer& createBufferParams)
            {
                RenderCommand cmd;

                ResourceSlot resource_slot = g_ctx.slot_resources.next_slot();

                cmd.command_type = commandType;
                cmd.resource_slot = resource_slot.slot_id();

                memcpy(&cmd.create_buffer_params, (void*)&createBufferParams, sizeof(commands::CreateConstantBuffer));

                if (!add_cmd(cmd))
                {
                    REX_ERROR(LogRendererCore, "Unable to add/exec create constant buffer command");
                    return ResourceSlot::make_invalid();
                }

                return resource_slot;
            }
        }

        //-------------------------------------------------------------------------
        bool exec_cmd(const RenderCommand& cmd)
        {
            bool result = true;

            switch (cmd.command_type)
            {
            case CommandType::CREATE_FRAME_RESOURCE:
                result = backend::create_frame_resource(cmd.resource_slot);
                break;

            case CommandType::NEW_FRAME:
                result = backend::new_frame();
                break;
            case CommandType::END_FRAME:
                result = backend::end_frame();
                break;

            case CommandType::PRESENT:
                result = backend::present();
                rsl::swap(globals::g_default_targets_info.front_buffer_color, globals::g_default_targets_info.back_buffer_color);
                break;

            case CommandType::USER_PREPARE:
                backend::prepare_user_initialization();
                break;
            case CommandType::USER_FINISH:
                backend::finish_user_initialization();
                break;

            }

            if (!result)
            {
                REX_ERROR(LogRendererCore, "Failed to execute render command: {}", rsl::enum_refl::enum_name(cmd.command_type));
                return false;
            }

            return true;
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
        ResourceSlot create_clear_state(const commands::ClearState& clearStateParams)
        {
            RenderCommand cmd;

            ResourceSlot resource_slot = g_ctx.slot_resources.next_slot();

            cmd.command_type = CommandType::CREATE_CLEAR_STATE;
            cmd.clear_state_params = clearStateParams;
            cmd.resource_slot = resource_slot.slot_id();

            if (!add_cmd(cmd))
            {
                REX_ERROR(LogRendererCore, "Unable to add/exec create clear state command");
                return ResourceSlot::make_invalid();
            }

            return resource_slot;
        }

        //-------------------------------------------------------------------------
        ResourceSlot create_raster_state(const commands::RasterState& rasterStateParams)
        {
            RenderCommand cmd;

            ResourceSlot resource_slot = g_ctx.slot_resources.next_slot();

            cmd.command_type = CommandType::CREATE_RASTER_STATE;
            cmd.resource_slot = resource_slot.slot_id();

            memcpy(&cmd.raster_state_params, (void*)&rasterStateParams, sizeof(commands::RasterState));

            if (!add_cmd(cmd))
            {
                REX_ERROR(LogRendererCore, "Unable to add/exec create raster state command");
                return ResourceSlot::make_invalid();
            }

            return resource_slot;
        }

        //-------------------------------------------------------------------------
        ResourceSlot create_input_layout(const commands::CreateInputLayout& createInputLayoutParams)
        {
            RenderCommand cmd;

            ResourceSlot resource_slot = g_ctx.slot_resources.next_slot();

            cmd.command_type = CommandType::CREATE_INPUT_LAYOUT_STATE;
            cmd.resource_slot = resource_slot.slot_id();

            cmd.create_input_layout_params.num_elements = createInputLayoutParams.num_elements;

            s32 input_layouts_size = sizeof(commands::InputLayoutDescription) * createInputLayoutParams.num_elements;
            cmd.create_input_layout_params.input_layout = (commands::InputLayoutDescription*)memory_alloc(input_layouts_size);
            memcpy(cmd.create_input_layout_params.input_layout, createInputLayoutParams.input_layout, input_layouts_size);

            if (!add_cmd(cmd))
            {
                REX_ERROR(LogRendererCore, "Unable to add/exec create input layout command");
                return ResourceSlot::make_invalid();
            }

            return resource_slot;
        }

        //-------------------------------------------------------------------------
        ResourceSlot create_vertex_buffer(const commands::CreateBuffer& createBufferParams)
        {
            return internal::create_buffer(CommandType::CREATE_VERTEX_BUFFER, createBufferParams);
        }

        //-------------------------------------------------------------------------
        ResourceSlot create_index_buffer(const commands::CreateBuffer& createBufferParams)
        {
            return internal::create_buffer(CommandType::CREATE_INDEX_BUFFER, createBufferParams);
        }

        //-------------------------------------------------------------------------
        ResourceSlot create_constant_buffer(const commands::CreateConstantBuffer& createBufferParams)
        {
            return internal::create_constant_buffer(CommandType::CREATE_CONSTANT_BUFFER, createBufferParams);
        }

        //-------------------------------------------------------------------------
        ResourceSlot create_pipeline_state_object(const commands::CreatePipelineState& createPipelineStateParams)
        {
            RenderCommand cmd;

            ResourceSlot resource_slot = g_ctx.slot_resources.next_slot();

            cmd.command_type = CommandType::CREATE_PIPELINE_STATE;
            cmd.resource_slot = resource_slot.slot_id();

            memcpy(&cmd.raster_state_params, (void*)&createPipelineStateParams, sizeof(commands::CreatePipelineState));

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
            RenderCommand cmd;

            ResourceSlot resource_slot = g_ctx.slot_resources.next_slot();

            cmd.command_type = CommandType::CREATE_FRAME_RESOURCE;
            cmd.resource_slot = resource_slot.slot_id();

            if (!add_cmd(cmd))
            {
                REX_ERROR(LogRendererCore, "Unable to add/exec create frame resource command");
                return ResourceSlot::make_invalid();
            }

            return resource_slot;
        }

        //-------------------------------------------------------------------------
        ResourceSlot load_shader(const commands::LoadShader& loadShaderParams)
        {
            RenderCommand cmd;

            ResourceSlot resource_slot = g_ctx.slot_resources.next_slot();

            cmd.command_type = CommandType::LOAD_SHADER;
            cmd.resource_slot = resource_slot.slot_id();

            cmd.load_shader_params.byte_code_size = loadShaderParams.byte_code_size;
            cmd.load_shader_params.shader_type = loadShaderParams.shader_type;

            if (loadShaderParams.byte_code)
            {
                cmd.load_shader_params.byte_code = memory_alloc(loadShaderParams.byte_code_size);
                memcpy(cmd.load_shader_params.byte_code, loadShaderParams.byte_code, loadShaderParams.byte_code_size);
            }

            if (!add_cmd(cmd))
            {
                REX_ERROR(LogRendererCore, "Unable to add/exec load shader command");
                return ResourceSlot::make_invalid();
            }

            return resource_slot;
        }

        //-------------------------------------------------------------------------
        ResourceSlot link_shader(const commands::LinkShader& linkShaderParams)
        {
            RenderCommand cmd;

            ResourceSlot resource_slot = g_ctx.slot_resources.next_slot();

            cmd.command_type = CommandType::LINK_SHADER;
            cmd.resource_slot = resource_slot.slot_id();

            cmd.link_shader_params = linkShaderParams;

            s32 num = linkShaderParams.num_constants;
            s32 layout_size = sizeof(commands::ConstantLayoutDescription) * num;
            cmd.link_shader_params.constants = (commands::ConstantLayoutDescription*)memory_alloc(layout_size);

            commands::ConstantLayoutDescription* c = cmd.link_shader_params.constants;
            for (s32 i = 0; i < num; ++i)
            {
                c[i].location = linkShaderParams.constants[i].location;
                c[i].type = linkShaderParams.constants[i].type;

                s32 len = rsl::strlen(linkShaderParams.constants[i].name);
                c[i].name = (char8*)memory_alloc(len + 1);

                memcpy(c[i].name, linkShaderParams.constants[i].name, len);
                c[i].name[len] = '\0';
            }

            if (!add_cmd(cmd))
            {
                REX_ERROR(LogRendererCore, "Unable to add/exec link shader command");
                return ResourceSlot::make_invalid();
            }

            return resource_slot;
        }

        //-------------------------------------------------------------------------
        ResourceSlot compile_shader(const commands::CompileShader& compileShaderParams)
        {
            RenderCommand cmd;

            ResourceSlot resource_slot = g_ctx.slot_resources.next_slot();

            cmd.command_type = CommandType::COMPILE_SHADER;
            cmd.resource_slot = resource_slot.slot_id();

            memcpy(&cmd.compile_shader_params, (void*)&compileShaderParams, sizeof(commands::CompileShader));

            if (!add_cmd(cmd))
            {
                REX_ERROR(LogRendererCore, "Unable to add/exec compile shader command");
                return ResourceSlot::make_invalid();
            }

            return resource_slot;
        }

        //-------------------------------------------------------------------------
        bool update_constant_buffer(const commands::UpdateConstantBuffer& updateConstantBufferParams, ResourceSlot constantBufferTarget)
        {
            RenderCommand cmd;

            cmd.command_type = CommandType::UPDATE_CONSTANT_BUFFER;
            cmd.resource_slot = constantBufferTarget.slot_id();

            if (updateConstantBufferParams.data)
            {
                cmd.update_constant_buffer_params.data = memory_alloc(updateConstantBufferParams.data_size);
                memcpy(cmd.update_constant_buffer_params.data, updateConstantBufferParams.data, updateConstantBufferParams.data_size);

                cmd.update_constant_buffer_params.data_size = updateConstantBufferParams.data_size;
                cmd.update_constant_buffer_params.element_index = updateConstantBufferParams.element_index;
            }

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
            RenderCommand cmd;

            cmd.command_type = CommandType::RELEASE_RESOURCE;
            cmd.release_resource.resource_index = resourceTarget;

            return add_cmd(cmd);
        }

        //-------------------------------------------------------------------------
        bool prepare_user_initialization()
        {
            RenderCommand cmd;

            cmd.command_type = CommandType::USER_PREPARE;

            return add_cmd(cmd);
        }

        //-------------------------------------------------------------------------
        bool finish_user_initialization()
        {
            RenderCommand cmd;

            cmd.command_type = CommandType::USER_FINISH;

            return add_cmd(cmd);
        }

        //-------------------------------------------------------------------------
        bool clear(ResourceSlot clearStateTarget)
        {
            RenderCommand cmd;

            cmd.command_type = CommandType::CLEAR;
            cmd.clear.clear_state = clearStateTarget;

            return add_cmd(cmd);
        }

        //-------------------------------------------------------------------------
        bool renderer_draw(s32 vertexCount, s32 startVertex)
        {
            RenderCommand cmd;

            cmd.command_type = CommandType::DRAW;
            cmd.draw.vertex_count = vertexCount;
            cmd.draw.start_vertex = startVertex;

            return add_cmd(cmd);
        }

        //-------------------------------------------------------------------------
        bool renderer_draw_indexed(s32 indexCount, s32 startIndex, s32 baseVertex)
        {
            RenderCommand cmd;

            cmd.command_type = CommandType::DRAW_INDEXED;
            cmd.draw_indexed.index_count = indexCount;
            cmd.draw_indexed.start_index = startIndex;
            cmd.draw_indexed.base_vertex = baseVertex;

            return add_cmd(cmd);
        }

        //-------------------------------------------------------------------------
        bool renderer_draw_indexed_instanced(s32 instanceCount, s32 startInstance, s32 indexCount, s32 startIndex, s32 baseVertex)
        {
            RenderCommand cmd;

            cmd.command_type = CommandType::DRAW_INDEXED_INSTANCED;
            cmd.draw_indexed_instanced.instance_count = instanceCount;
            cmd.draw_indexed_instanced.start_instance = startInstance;
            cmd.draw_indexed_instanced.index_count = indexCount;
            cmd.draw_indexed_instanced.start_index = startIndex;
            cmd.draw_indexed_instanced.base_vertex = baseVertex;

            return add_cmd(cmd);
        }

        //-------------------------------------------------------------------------
        bool renderer_draw_instanced(s32 vertexCount, s32 instanceCount, s32 startVertex, s32 startInstance)
        {
            RenderCommand cmd;

            cmd.command_type = CommandType::DRAW_INSTANCED;
            cmd.draw_instanced.instance_count = instanceCount;
            cmd.draw_instanced.start_instance = startInstance;
            cmd.draw_instanced.vertex_count = vertexCount;
            cmd.draw_instanced.start_vertex = startVertex;

            return add_cmd(cmd);
        }

        //-------------------------------------------------------------------------
        bool set_raster_state(ResourceSlot rasterStateTarget)
        {
            RenderCommand cmd;

            cmd.command_type = CommandType::SET_RASTER_STATE;
            cmd.set_raster_state.raster_state = rasterStateTarget;

            return add_cmd(cmd);
        }

        //-------------------------------------------------------------------------
        bool set_render_targets(ResourceSlot* colorTargets, s32 numColorTargets, ResourceSlot depthTarget)
        {
            RenderCommand cmd;

            cmd.command_type = CommandType::SET_RENDER_TARGETS;
            cmd.set_render_target.num_color = numColorTargets;
            memcpy(&cmd.set_render_target.color, colorTargets, numColorTargets * sizeof(ResourceSlot));
            cmd.set_render_target.depth = depthTarget;

            return add_cmd(cmd);
        }

        //-------------------------------------------------------------------------
        bool set_render_targets(ResourceSlot colorTarget, ResourceSlot depthTarget)
        {
            RenderCommand cmd;

            cmd.command_type = CommandType::SET_RENDER_TARGETS;
            cmd.set_render_target.num_color = colorTarget.is_valid() ? 1 : 0;
            cmd.set_render_target.color[0] = colorTarget;
            cmd.set_render_target.depth = depthTarget;

            return add_cmd(cmd);
        }

        //-------------------------------------------------------------------------
        bool set_viewport(const Viewport& vp)
        {
            RenderCommand cmd;

            cmd.command_type = CommandType::SET_VIEWPORT;
            memcpy(&cmd.viewport, (void*)&vp, sizeof(Viewport));

            return add_cmd(cmd);
        }

        //-------------------------------------------------------------------------
        bool set_scissor_rect(const ScissorRect& sr)
        {
            RenderCommand cmd;

            cmd.command_type = CommandType::SET_SCISSOR_RECT;
            memcpy(&cmd.scissor_rect, (void*)&sr, sizeof(ScissorRect));

            return add_cmd(cmd);
        }

        //-------------------------------------------------------------------------
        bool set_input_layout(ResourceSlot inputLayoutTarget)
        {
            RenderCommand cmd;

            cmd.command_type = CommandType::SET_INPUT_LAYOUT;
            cmd.set_input_layout.input_layout = inputLayoutTarget;

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
            RenderCommand cmd;

            cmd.command_type = CommandType::SET_VERTEX_BUFFER;

            cmd.set_vertex_buffer.start_slot = startSlot;
            cmd.set_vertex_buffer.num_buffers = numBuffers;

            cmd.set_vertex_buffer.buffer_indices = (ResourceSlot*)memory_alloc(sizeof(s32) * numBuffers);
            cmd.set_vertex_buffer.strides = (s32*)memory_alloc(sizeof(s32) * numBuffers);
            cmd.set_vertex_buffer.offsets = (s32*)memory_alloc(sizeof(s32) * numBuffers);

            for (s32 i = 0; i < numBuffers; ++i)
            {
                cmd.set_vertex_buffer.buffer_indices[i] = vertexBufferTargets[i];
                cmd.set_vertex_buffer.strides[i] = strides[i];
                cmd.set_vertex_buffer.offsets[i] = offsets[i];
            }

            return add_cmd(cmd);
        }
        //-------------------------------------------------------------------------
        bool set_index_buffer(ResourceSlot indexBufferTarget, IndexBufferFormat format, s32 offset)
        {
            RenderCommand cmd;

            cmd.command_type = CommandType::SET_INDEX_BUFFER;
            cmd.set_index_buffer.buffer_index = indexBufferTarget;
            cmd.set_index_buffer.format = format;
            cmd.set_index_buffer.offset = offset;

            return add_cmd(cmd);
        }

        //-------------------------------------------------------------------------
        bool set_shader(ResourceSlot shaderTarget)
        {
            RenderCommand cmd;

            cmd.command_type = CommandType::SET_SHADER;

            cmd.set_shader.shader_index = shaderTarget;

            return add_cmd(cmd);
        }

        //-------------------------------------------------------------------------
        bool set_pipeline_state_object(ResourceSlot psoTarget)
        {
            RenderCommand cmd;

            cmd.command_type = CommandType::SET_PIPELINE_STATE;

            cmd.set_pipeline_state.pipeline_state = psoTarget;

            return add_cmd(cmd);
        }

        //-------------------------------------------------------------------------
        bool set_constant_buffer(ResourceSlot constantBufferTarget, s32 location)
        {
            RenderCommand cmd;

            cmd.command_type = CommandType::SET_CONSTANT_BUFFER;

            cmd.set_constant_buffer.target = constantBufferTarget;
            cmd.set_constant_buffer.location = location;

            return add_cmd(cmd);
        }

        //-------------------------------------------------------------------------
        bool set_primitive_topology(PrimitiveTopology primitiveTopology)
        {
            RenderCommand cmd;

            cmd.command_type = CommandType::SET_PRIMITIVE_TOPOLOGY;

            cmd.topology = primitiveTopology;

            return add_cmd(cmd);
        }

        //-------------------------------------------------------------------------
        bool new_frame()
        {
            RenderCommand cmd;

            cmd.command_type = CommandType::NEW_FRAME;

            return add_cmd(cmd);
        }

        //-------------------------------------------------------------------------
        bool end_frame()
        {
            RenderCommand cmd;

            cmd.command_type = CommandType::END_FRAME;

            return add_cmd(cmd);
        }

        //-------------------------------------------------------------------------
        bool begin_draw()
        {
            RenderCommand cmd;

            cmd.command_type = CommandType::BEGIN_DRAW;

            return add_cmd(cmd);
        }

        //-------------------------------------------------------------------------
        bool end_draw()
        {
            RenderCommand cmd;

            cmd.command_type = CommandType::END_DRAW;

            return add_cmd(cmd);
        }

        //-------------------------------------------------------------------------
        bool present()
        {
            RenderCommand cmd;

            cmd.command_type = CommandType::PRESENT;

            return add_cmd(cmd);
        }

        //-------------------------------------------------------------------------
        bool flush()
        {
            RenderCommand* cmd = g_ctx.cmd_buffer.get();

            while (cmd)
            {
                if (exec_cmd(*cmd) == false)
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