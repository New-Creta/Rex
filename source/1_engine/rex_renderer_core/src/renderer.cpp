#include "rex_renderer_core/renderer.h"
#include "rex_renderer_core/commands/render_cmd.h"
#include "rex_renderer_core/resource_slots.h"
#include "rex_renderer_core/log.h"
#include "rex_engine/defines.h"
#include "rex_engine/ring_buffer.h"
#include "rex_renderer_core/commands/render_cmd.h"

#if REX_SINGLE_THREADED
#define add_cmd(cmd) exec_cmd(cmd)
#else
#define add_cmd(cmd) g_ctx.cmd_buffer.put(cmd)
#endif

namespace rex
{
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
            s32 create_buffer(CommandType commandType, const parameters::CreateBuffer& createBufferParams)
            {
                RenderCommand cmd;

                s32 resource_slot = g_ctx.slot_resources.next_slot();

                cmd.command_type = commandType;
                cmd.resource_slot = resource_slot;

                memcpy(&cmd.create_buffer_params, (void*)&createBufferParams, sizeof(parameters::CreateBuffer));

                if (createBufferParams.data)
                {
                    // make a copy of the buffers data
                    cmd.create_buffer_params.data = memory_alloc(createBufferParams.buffer_size);
                    memcpy(cmd.create_buffer_params.data, createBufferParams.data, createBufferParams.buffer_size);
                }

                add_cmd(cmd);

                return resource_slot;
            }

            //-------------------------------------------------------------------------
            s32 create_constant_buffer(CommandType commandType, const parameters::CreateConstantBuffer& createBufferParams)
            {
                RenderCommand cmd;

                s32 resource_slot = g_ctx.slot_resources.next_slot();

                cmd.command_type = commandType;
                cmd.resource_slot = resource_slot;

                memcpy(&cmd.create_buffer_params, (void*)&createBufferParams, sizeof(parameters::CreateConstantBuffer));

                if (createBufferParams.data)
                {
                    // make a copy of the buffers data
                    cmd.create_buffer_params.data = memory_alloc(createBufferParams.buffer_size);
                    memcpy(cmd.create_buffer_params.data, createBufferParams.data, createBufferParams.buffer_size);
                }

                add_cmd(cmd);

                return resource_slot;
            }
        }

        //-------------------------------------------------------------------------
        void exec_cmd(const RenderCommand& cmd)
        {
            bool result = true;

            switch (cmd.command_type)
            {
            case CommandType::CREATE_CLEAR_STATE:
                result = backend::create_clear_state(cmd.clear_state_params, cmd.resource_slot);
                break;
            case CommandType::CREATE_RASTER_STATE:
                result = backend::create_raster_state(cmd.raster_state_params, cmd.resource_slot);
                break;
            case CommandType::CREATE_INPUT_LAYOUT_STATE:
                result = backend::create_input_layout(cmd.create_input_layout_params, cmd.resource_slot);
                memory_free(cmd.create_input_layout_params.input_layout);
                break;
            case CommandType::CREATE_VERTEX_BUFFER:
                result = backend::create_vertex_buffer(cmd.create_buffer_params, cmd.resource_slot);
                memory_free(cmd.create_buffer_params.data);
                break;
            case CommandType::CREATE_INDEX_BUFFER:
                result = backend::create_index_buffer(cmd.create_buffer_params, cmd.resource_slot);
                memory_free(cmd.create_buffer_params.data);
                break;
            case CommandType::CREATE_CONSTANT_BUFFER:
                result = backend::create_constant_buffer(cmd.create_constant_buffer_params, cmd.resource_slot);
                memory_free(cmd.create_buffer_params.data);
                break;
            case CommandType::CREATE_PIPELINE_STATE:
                result = backend::create_pipeline_state_object(cmd.create_pipeline_state_params, cmd.resource_slot);
            case CommandType::LOAD_SHADER:
                result = backend::load_shader(cmd.load_shader_params, cmd.resource_slot);
                memory_free(cmd.load_shader_params.byte_code);
                break;
            case CommandType::LINK_SHADER:
                result = backend::link_shader(cmd.link_shader_params, cmd.resource_slot);
                for (s32 i = 0; i < cmd.link_shader_params.num_constants; ++i)
                    memory_free(cmd.link_shader_params.constants[i].name);
                memory_free(cmd.link_shader_params.constants);
                break;

            case CommandType::RELEASE_RESOURCE:
                result = backend::release_resource(cmd.release_resource.resource_index);
                break;

            case CommandType::CLEAR:
                backend::clear(cmd.clear.clear_state);
                break;

            case CommandType::DRAW:
                backend::draw(cmd.draw.vertex_count
                    , cmd.draw.start_vertex
                    , cmd.draw.primitive_topology);
                break;
            case CommandType::DRAW_INDEXED:
                backend::draw_indexed(cmd.draw_indexed.index_count
                    , cmd.draw_indexed.start_index
                    , cmd.draw_indexed.base_vertex
                    , cmd.draw_indexed.primitive_topology);
                break;
            case CommandType::DRAW_INDEXED_INSTANCED:
                backend::draw_indexed_instanced(cmd.draw_indexed_instanced.instance_count
                    , cmd.draw_indexed_instanced.start_instance
                    , cmd.draw_indexed_instanced.index_count
                    , cmd.draw_indexed_instanced.start_index
                    , cmd.draw_indexed_instanced.base_vertex
                    , cmd.draw_indexed_instanced.primitive_topology);
                break;

            case CommandType::SET_RENDER_TARGETS:
                result = backend::set_render_targets(cmd.set_render_target.color
                    , cmd.set_render_target.num_color
                    , cmd.set_render_target.depth);
                break;
            case CommandType::SET_INPUT_LAYOUT:
                result = backend::set_input_layout(cmd.set_input_layout.input_layout);
                break;
            case CommandType::SET_VIEWPORT:
                result = backend::set_viewport(cmd.viewport);
                break;
            case CommandType::SET_SCISSOR_RECT:
                result = backend::set_scissor_rect(cmd.scissor_rect);
                break;
            case CommandType::SET_VERTEX_BUFFER:
                result = backend::set_vertex_buffers(cmd.set_vertex_buffer.buffer_indices
                    , cmd.set_vertex_buffer.num_buffers
                    , cmd.set_vertex_buffer.start_slot
                    , cmd.set_vertex_buffer.strides
                    , cmd.set_vertex_buffer.offsets);
                memory_free(cmd.set_vertex_buffer.buffer_indices);
                memory_free(cmd.set_vertex_buffer.strides);
                memory_free(cmd.set_vertex_buffer.offsets);
                break;
            case CommandType::SET_INDEX_BUFFER:
                result = backend::set_index_buffer(cmd.set_index_buffer.buffer_index
                    , cmd.set_index_buffer.format
                    , cmd.set_index_buffer.offset);
                break;
            case CommandType::SET_SHADER:
                result = backend::set_shader(cmd.set_shader.shader_index, cmd.set_shader.shader_type);
                break;
            case CommandType::SET_RASTER_STATE:
                result = backend::set_raster_state(cmd.set_raster_state.raster_state);
                break;
            case CommandType::SET_PIPELINE_STATE:
                result = backend::set_pipeline_state_object(cmd.set_pipeline_state.pipeline_state);
                break;

            case CommandType::NEW_FRAME:
                //new_frame_internal();
                break;
            case CommandType::END_FRAME:
                //end_frame_internal();
                break;

            case CommandType::PRESENT:
                result = backend::present();
                break;

            }

            if (!result)
            {
                REX_ERROR(LogRendererCore, "Failed to execute render command: {}", (s32)cmd.command_type);
            }
        }

        //-------------------------------------------------------------------------
        bool initialize(const OutputWindowUserData& userData, s32 maxCommands)
        {
            UNUSED_PARAM(userData);

            g_ctx.slot_resources.initialize(32);
            g_ctx.cmd_buffer.initialize(maxCommands);

            return backend::initialize(userData
                , (s32)DefaultTargets::REX_FRONT_BUFFER_COLOR
                , (s32)DefaultTargets::REX_BACK_BUFFER_COLOR
                , (s32)DefaultTargets::REX_BUFFER_DEPTH);
        }
        //-------------------------------------------------------------------------
        void shutdown()
        {
            backend::shutdown();
        }

        //-------------------------------------------------------------------------
        s32 create_clear_state(const parameters::ClearState& clearStateParams)
        {
            RenderCommand cmd;

            s32 resource_slot = g_ctx.slot_resources.next_slot();

            cmd.command_type = CommandType::CREATE_CLEAR_STATE;
            cmd.clear_state_params = clearStateParams;
            cmd.resource_slot = resource_slot;

            add_cmd(cmd);

            return resource_slot;
        }

        //-------------------------------------------------------------------------
        s32 create_raster_state(const parameters::RasterState& rasterStateParams)
        {
            RenderCommand cmd;

            s32 resource_slot = g_ctx.slot_resources.next_slot();

            cmd.command_type = CommandType::CREATE_RASTER_STATE;
            cmd.resource_slot = resource_slot;

            memcpy(&cmd.raster_state_params, (void*)&rasterStateParams, sizeof(parameters::RasterState));

            add_cmd(cmd);

            return resource_slot;
        }

        //-------------------------------------------------------------------------
        s32 create_input_layout(const parameters::CreateInputLayout& createInputLayoutParams)
        {
            RenderCommand cmd;

            s32 resource_slot = g_ctx.slot_resources.next_slot();

            cmd.command_type = CommandType::CREATE_INPUT_LAYOUT_STATE;
            cmd.resource_slot = resource_slot;

            cmd.create_input_layout_params.num_elements = createInputLayoutParams.num_elements;

            s32 input_layouts_size = sizeof(parameters::InputLayoutDescription) * createInputLayoutParams.num_elements;
            cmd.create_input_layout_params.input_layout = (parameters::InputLayoutDescription*)memory_alloc(input_layouts_size);
            memcpy(cmd.create_input_layout_params.input_layout, createInputLayoutParams.input_layout, input_layouts_size);

            add_cmd(cmd);

            return resource_slot;
        }

        //-------------------------------------------------------------------------
        s32 create_vertex_buffer(const parameters::CreateBuffer& createBufferParams)
        {
            return internal::create_buffer(CommandType::CREATE_VERTEX_BUFFER, createBufferParams);
        }

        //-------------------------------------------------------------------------
        s32 create_index_buffer(const parameters::CreateBuffer& createBufferParams)
        {
            return internal::create_buffer(CommandType::CREATE_INDEX_BUFFER, createBufferParams);
        }

        //-------------------------------------------------------------------------
        s32 create_constant_buffer(const parameters::CreateConstantBuffer& createBufferParams)
        {
            return internal::create_constant_buffer(CommandType::CREATE_CONSTANT_BUFFER, createBufferParams);
        }

        s32 create_pipeline_state_object(const parameters::CreatePipelineState& createPipelineStateParams)
        {
            RenderCommand cmd;

            s32 resource_slot = g_ctx.slot_resources.next_slot();

            cmd.command_type = CommandType::CREATE_PIPELINE_STATE;
            cmd.resource_slot = resource_slot;

            memcpy(&cmd.raster_state_params, (void*)&createPipelineStateParams, sizeof(parameters::CreatePipelineState));

            add_cmd(cmd);

            return resource_slot;
        }

        //-------------------------------------------------------------------------
        s32 load_shader(const parameters::LoadShader& loadShaderParams)
        {
            RenderCommand cmd;

            s32 resource_slot = g_ctx.slot_resources.next_slot();

            cmd.command_type = CommandType::LOAD_SHADER;
            cmd.resource_slot = resource_slot;

            cmd.load_shader_params.byte_code_size = loadShaderParams.byte_code_size;
            cmd.load_shader_params.shader_type = loadShaderParams.shader_type;

            if (loadShaderParams.byte_code)
            {
                cmd.load_shader_params.byte_code = memory_alloc(loadShaderParams.byte_code_size);
                memcpy(cmd.load_shader_params.byte_code, loadShaderParams.byte_code, loadShaderParams.byte_code_size);
            }

            add_cmd(cmd);

            return resource_slot;
        }

        //-------------------------------------------------------------------------
        s32 link_shader(const parameters::LinkShader& linkShaderParams)
        {
            RenderCommand cmd;

            s32 resource_slot = g_ctx.slot_resources.next_slot();

            cmd.command_type = CommandType::LINK_SHADER;
            cmd.resource_slot = resource_slot;

            cmd.link_shader_params = linkShaderParams;

            s32 num = linkShaderParams.num_constants;
            s32 layout_size = sizeof(parameters::ConstantLayoutDescription) * num;
            cmd.link_shader_params.constants = (parameters::ConstantLayoutDescription*)memory_alloc(layout_size);

            parameters::ConstantLayoutDescription* c = cmd.link_shader_params.constants;
            for (s32 i = 0; i < num; ++i)
            {
                c[i].location = linkShaderParams.constants[i].location;
                c[i].type = linkShaderParams.constants[i].type;

                s32 len = rsl::strlen(linkShaderParams.constants[i].name);
                c[i].name = (char8*)memory_alloc(len + 1);

                memcpy(c[i].name, linkShaderParams.constants[i].name, len);
                c[i].name[len] = '\0';
            }

            add_cmd(cmd);

            return resource_slot;
        }

        //-------------------------------------------------------------------------
        s32 compile_shader(const parameters::CompileShader& compileShaderParams)
        {
            RenderCommand cmd;

            s32 resource_slot = g_ctx.slot_resources.next_slot();

            cmd.command_type = CommandType::COMPILE_SHADER;
            cmd.resource_slot = resource_slot;

            memcpy(&cmd.compile_shader_params, (void*)&compileShaderParams, sizeof(parameters::CompileShader));

            add_cmd(cmd);

            return resource_slot;
        }

        //-------------------------------------------------------------------------
        void release_resource(s32 resourceTarget)
        {
            RenderCommand cmd;

            cmd.command_type = CommandType::RELEASE_RESOURCE;
            cmd.release_resource.resource_index = resourceTarget;

            add_cmd(cmd);
        }

        //-------------------------------------------------------------------------
        void clear(s32 clearStateTarget)
        {
            RenderCommand cmd;

            cmd.command_type = CommandType::CLEAR;
            cmd.clear.clear_state = clearStateTarget;

            add_cmd(cmd);
        }

        //-------------------------------------------------------------------------
        void renderer_draw(s32 vertexCount, s32 startVertex, PrimitiveTopology primitiveTopology)
        {
            RenderCommand cmd;

            cmd.command_type = CommandType::DRAW;
            cmd.draw.vertex_count = vertexCount;
            cmd.draw.start_vertex = startVertex;
            cmd.draw.primitive_topology = primitiveTopology;

            add_cmd(cmd);
        }

        //-------------------------------------------------------------------------
        void renderer_draw_indexed(s32 indexCount, s32 startIndex, s32 baseVertex, PrimitiveTopology primitiveTopology)
        {
            RenderCommand cmd;

            cmd.command_type = CommandType::DRAW_INDEXED;
            cmd.draw_indexed.index_count = indexCount;
            cmd.draw_indexed.start_index = startIndex;
            cmd.draw_indexed.base_vertex = baseVertex;
            cmd.draw_indexed.primitive_topology = primitiveTopology;

            add_cmd(cmd);
        }

        //-------------------------------------------------------------------------
        void renderer_draw_indexed_instanced(s32 instanceCount, s32 startInstance, s32 indexCount, s32 startIndex, s32 baseVertex, PrimitiveTopology primitiveTopology)
        {
            RenderCommand cmd;

            cmd.command_type = CommandType::DRAW_INDEXED_INSTANCED;
            cmd.draw_indexed_instanced.instance_count = instanceCount;
            cmd.draw_indexed_instanced.start_instance = startInstance;
            cmd.draw_indexed_instanced.index_count = indexCount;
            cmd.draw_indexed_instanced.start_index = startIndex;
            cmd.draw_indexed_instanced.base_vertex = baseVertex;
            cmd.draw_indexed_instanced.primitive_topology = primitiveTopology;

            add_cmd(cmd);
        }

        //-------------------------------------------------------------------------
        void renderer_draw_instanced(s32 vertexCount, s32 instanceCount, s32 startVertex, s32 startInstance, PrimitiveTopology topology)
        {
            RenderCommand cmd;

            cmd.command_type = CommandType::DRAW_INSTANCED;
            cmd.draw_instanced.instance_count = instanceCount;
            cmd.draw_instanced.start_instance = startInstance;
            cmd.draw_instanced.vertex_count = vertexCount;
            cmd.draw_instanced.start_vertex = startVertex;
            cmd.draw_instanced.primitive_topology = topology;

            add_cmd(cmd);
        }

        //-------------------------------------------------------------------------
        void set_raster_state(s32 rasterStateTarget)
        {
            RenderCommand cmd;

            cmd.command_type = CommandType::SET_RASTER_STATE;
            cmd.set_raster_state.raster_state = rasterStateTarget;

            add_cmd(cmd);
        }

        //-------------------------------------------------------------------------
        void set_render_targets(s32* colorTargets, s32 numColorTargets, s32 depthTarget)
        {
            RenderCommand cmd;

            cmd.command_type = CommandType::SET_RENDER_TARGETS;
            cmd.set_render_target.num_color = numColorTargets;
            memcpy(&cmd.set_render_target.color, colorTargets, numColorTargets * sizeof(s32));
            cmd.set_render_target.depth = depthTarget;

            add_cmd(cmd);
        }

        //-------------------------------------------------------------------------
        void set_render_targets(s32 colorTarget, s32 depthTarget)
        {
            RenderCommand cmd;

            cmd.command_type = CommandType::SET_RENDER_TARGETS;
            cmd.set_render_target.num_color = is_valid(colorTarget) ? 1 : 0;
            cmd.set_render_target.color[0] = colorTarget;
            cmd.set_render_target.depth = depthTarget;

            add_cmd(cmd);
        }

        //-------------------------------------------------------------------------
        void set_viewport(const Viewport& vp)
        {
            RenderCommand cmd;

            cmd.command_type = CommandType::SET_VIEWPORT;
            memcpy(&cmd.viewport, (void*)&vp, sizeof(Viewport));

            add_cmd(cmd);
        }

        //-------------------------------------------------------------------------
        void set_scissor_rect(const ScissorRect& sr)
        {
            RenderCommand cmd;

            cmd.command_type = CommandType::SET_SCISSOR_RECT;
            memcpy(&cmd.scissor_rect, (void*)&sr, sizeof(ScissorRect));

            add_cmd(cmd);
        }

        //-------------------------------------------------------------------------
        void set_input_layout(s32 inputLayoutTarget)
        {
            RenderCommand cmd;

            cmd.command_type = CommandType::SET_INPUT_LAYOUT;
            cmd.set_input_layout.input_layout = inputLayoutTarget;

            add_cmd(cmd);
        }

        //-------------------------------------------------------------------------
        void set_vertex_buffer(s32 vertexBufferTarget, s32 startSlot, s32 stride, s32 offset)
        {
            s32* vertex_buffer_target = &vertexBufferTarget;
            const s32* stride_target = &stride;
            const s32* offset_target = &offset;

            set_vertex_buffers(vertex_buffer_target, 1, startSlot, stride_target, offset_target);
        }
        //-------------------------------------------------------------------------
        void set_vertex_buffers(s32* vertexBufferTargets, s32 numBuffers, s32 startSlot, const s32* strides, const s32* offsets)
        {
            RenderCommand cmd;

            cmd.command_type = CommandType::SET_VERTEX_BUFFER;

            cmd.set_vertex_buffer.start_slot = startSlot;
            cmd.set_vertex_buffer.num_buffers = numBuffers;

            cmd.set_vertex_buffer.buffer_indices = (s32*)memory_alloc(sizeof(s32) * numBuffers);
            cmd.set_vertex_buffer.strides = (s32*)memory_alloc(sizeof(s32) * numBuffers);
            cmd.set_vertex_buffer.offsets = (s32*)memory_alloc(sizeof(s32) * numBuffers);

            for (s32 i = 0; i < numBuffers; ++i)
            {
                cmd.set_vertex_buffer.buffer_indices[i] = vertexBufferTargets[i];
                cmd.set_vertex_buffer.strides[i] = strides[i];
                cmd.set_vertex_buffer.offsets[i] = offsets[i];
            }

            add_cmd(cmd);
        }
        //-------------------------------------------------------------------------
        void set_index_buffer(s32 indexBufferTarget, IndexBufferFormat format, s32 offset)
        {
            RenderCommand cmd;

            cmd.command_type = CommandType::SET_INDEX_BUFFER;
            cmd.set_index_buffer.buffer_index = indexBufferTarget;
            cmd.set_index_buffer.format = format;
            cmd.set_index_buffer.offset = offset;

            add_cmd(cmd);
        }

        //-------------------------------------------------------------------------
        void set_shader(s32 shaderTarget, ShaderType shaderType)
        {
            RenderCommand cmd;

            cmd.command_type = CommandType::SET_SHADER;

            cmd.set_shader.shader_index = shaderTarget;
            cmd.set_shader.shader_type = shaderType;

            add_cmd(cmd);
        }

        //-------------------------------------------------------------------------
        void set_pipeline_state_object(s32 psoTarget)
        {
            RenderCommand cmd;

            cmd.command_type = CommandType::SET_PIPELINE_STATE;

            cmd.set_pipeline_state.pipeline_state = psoTarget;

            add_cmd(cmd);
        }

        //-------------------------------------------------------------------------
        void new_frame()
        {
            RenderCommand cmd;

            cmd.command_type = CommandType::NEW_FRAME;

            add_cmd(cmd);
        }

        //-------------------------------------------------------------------------
        void end_frame()
        {
            RenderCommand cmd;

            cmd.command_type = CommandType::END_FRAME;

            add_cmd(cmd);
        }

        //-------------------------------------------------------------------------
        void present()
        {
            RenderCommand cmd;

            cmd.command_type = CommandType::PRESENT;

            add_cmd(cmd);
        }

    } // namespace renderer
} // namespace rex