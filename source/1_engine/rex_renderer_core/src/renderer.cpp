#include "rex_renderer_core/renderer.h"
#include "rex_renderer_core/commands/render_cmd.h"
#include "rex_renderer_core/resource_slots.h"
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


        void exec_cmd(const RenderCommand& cmd)
        {
            switch (cmd.command_type)
            {
            case CommandType::CREATE_CLEAR_STATE:
                backend::create_clear_state(cmd.clear_state_params, cmd.resource_slot);
                break;
            case CommandType::CREATE_RASTER_STATE:
                backend::create_raster_state(cmd.raster_state_params, cmd.resource_slot);
                break;
            case CommandType::CREATE_INPUT_LAYOUT_STATE:
                backend::create_input_layout(cmd.create_input_layout_params, cmd.resource_slot);
                memory_free(cmd.create_input_layout_params.vs_byte_code);
                memory_free(cmd.create_input_layout_params.input_layout);
                break;
            case CommandType::CREATE_BUFFER:
                backend::create_buffer(cmd.create_buffer_params, cmd.resource_slot);
                memory_free(cmd.create_buffer.data);
                break;
            case CommandType::LOAD_SHADER:
                backend::load_shader(cmd.load_shader_params, cmd.resource_slot);
                memory_free(cmd.load_shader_params.byte_code);
                break;
            case CommandType::LINK_SHADER:
                backend::link_shader_program(cmd.link_shader_params, cmd.resource_slot);
                for (u32 i = 0; i < cmd.link_shader_params.num_constants; ++i)
                    memory_free(cmd.link_shader_params.constants[i].name);
                memory_free(cmd.link_shader_params.constants);
                break;

            case CommandType::RELEASE_RESOURCE:
                backend::release_resource(cmd.release_resource.resource_index);
                break;

            case CommandType::CLEAR:
                backend::clear(cmd.clear.clear_state, cmd.clear.array_index, cmd.clear.array_index);
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
                backend::set_render_targets(cmd.set_render_target.color
                    , cmd.set_render_target.num_color
                    , cmd.set_render_target.depth
                    , cmd.set_render_target.array_index
                    , cmd.set_render_target.array_index);
                break;
            case CommandType::SET_INPUT_LAYOUT:
                backend::set_input_layout(cmd.set_input_layout.input_layout);
                break;
            case CommandType::SET_VIEWPORT:
                backend::set_viewport(cmd.viewport);
                break;
            case CommandType::SET_SCISSOR_RECT:
                backend::set_scissor_rect(cmd.scissor_rect);
                break;
            case CommandType::SET_VERTEX_BUFFER:
                backend::set_vertex_buffers(cmd.set_vertex_buffer.buffer_indices
                    , cmd.set_vertex_buffer.num_buffers
                    , cmd.set_vertex_buffer.start_slot
                    , cmd.set_vertex_buffer.strides
                    , cmd.set_vertex_buffer.offsets);
                memory_free(cmd.set_vertex_buffer.buffer_indices);
                memory_free(cmd.set_vertex_buffer.strides);
                memory_free(cmd.set_vertex_buffer.offsets);
                break;
            case CommandType::SET_INDEX_BUFFER:
                backend::set_index_buffer(cmd.set_index_buffer.buffer_index
                    , cmd.set_index_buffer.format
                    , cmd.set_index_buffer.offset);
                break;
            case CommandType::SET_SHADER:
                backend::set_shader(cmd.set_shader.shader_index, cmd.set_shader.shader_type);
                break;
            case CommandType::SET_RASTER_STATE:
                backend::set_raster_state(cmd.set_raster_state.raster_state);
                break;

            case CommandType::NEW_FRAME:
                //new_frame_internal();
                break;
            case CommandType::END_FRAME:
                //end_frame_internal();
                break;

            case CommandType::PRESENT:
                backend::present();
                break;

            }
        }

        //-------------------------------------------------------------------------
        bool initialize(const OutputWindowUserData& userData, u32 maxCommands)
        {
            UNUSED_PARAM(userData);

            g_ctx.slot_resources.initialize(32);
            g_ctx.cmd_buffer.initialize(maxCommands);

            return backend::initialize(userData);
        }
        //-------------------------------------------------------------------------
        void shutdown()
        {
            backend::shutdown();
        }

        //-------------------------------------------------------------------------
        u32 create_clear_state(const parameters::ClearState& clearStateParams)
        {
            RenderCommand cmd;

            u32 resource_slot = g_ctx.slot_resources.next_slot();

            cmd.command_type = CommandType::CREATE_CLEAR_STATE;
            cmd.clear_state_params = clearStateParams;
            cmd.resource_slot = resource_slot;

            add_cmd(cmd);

            return resource_slot;
        }

        //-------------------------------------------------------------------------
        u32 create_raster_state(const parameters::RasterState& rasterStateParams)
        {
            RenderCommand cmd;

            u32 resource_slot = g_ctx.slot_resources.next_slot();

            cmd.command_type = CommandType::CREATE_RASTER_STATE;
            cmd.resource_slot = resource_slot;

            memcpy(&cmd.raster_state_params, (void*)&rasterStateParams, sizeof(parameters::RasterState));

            add_cmd(cmd);

            return resource_slot;
        }

        //-------------------------------------------------------------------------
        u32 create_input_layout(const parameters::CreateInputLayout& createInputLayoutParams)
        {
            RenderCommand cmd;

            u32 resource_slot = g_ctx.slot_resources.next_slot();

            cmd.command_type = CommandType::CREATE_INPUT_LAYOUT_STATE;
            cmd.resource_slot = resource_slot;

            cmd.create_input_layout_params.num_elements = createInputLayoutParams.num_elements;
            cmd.create_input_layout_params.vs_byte_code_size = createInputLayoutParams.vs_byte_code_size;

            cmd.create_input_layout_params.vs_byte_code = memory_alloc(createInputLayoutParams.vs_byte_code_size);
            memcpy(cmd.create_input_layout_params.vs_byte_code, createInputLayoutParams.vs_byte_code, createInputLayoutParams.vs_byte_code_size);

            u32 input_layouts_size = sizeof(parameters::InputLayoutDescription) * createInputLayoutParams.num_elements;
            cmd.create_input_layout_params.input_layout = (parameters::InputLayoutDescription*)memory_alloc(input_layouts_size);
            memcpy(cmd.create_input_layout_params.input_layout, createInputLayoutParams.input_layout, input_layouts_size);

            add_cmd(cmd);

            return resource_slot;
        }

        //-------------------------------------------------------------------------
        u32 create_buffer(const parameters::CreateBuffer& createBufferParams)
        {
            RenderCommand cmd;

            u32 resource_slot = g_ctx.slot_resources.next_slot();

            cmd.command_type = CommandType::CREATE_BUFFER;
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
        u32 load_shader(const parameters::LoadShader& loadShaderParams)
        {
            RenderCommand cmd;

            u32 resource_slot = g_ctx.slot_resources.next_slot();

            cmd.command_type = CommandType::LOAD_SHADER;
            cmd.resource_slot = resource_slot;

            cmd.load_shader_params.byte_code_size = loadShaderParams.byte_code_size;
            cmd.load_shader_params.type = loadShaderParams.type;

            if (loadShaderParams.byte_code)
            {
                cmd.load_shader_params.byte_code = memory_alloc(loadShaderParams.byte_code_size);
                memcpy(cmd.load_shader_params.byte_code, loadShaderParams.byte_code, loadShaderParams.byte_code_size);
            }

            add_cmd(cmd);

            return resource_slot;
        }

        //-------------------------------------------------------------------------
        u32 link_shader(const parameters::LinkShader& linkShaderParams)
        {
            RenderCommand cmd;

            u32 resource_slot = g_ctx.slot_resources.next_slot();

            cmd.command_type = CommandType::LINK_SHADER;
            cmd.resource_slot = resource_slot;

            cmd.link_shader_params = linkShaderParams;

            u32 num = linkShaderParams.num_constants;
            u32 layout_size = sizeof(parameters::ConstantLayoutDescription) * num;
            cmd.link_shader_params.constants = (parameters::ConstantLayoutDescription*)memory_alloc(layout_size);

            parameters::ConstantLayoutDescription* c = cmd.link_shader_params.constants;
            for (u32 i = 0; i < num; ++i)
            {
                c[i].location = linkShaderParams.constants[i].location;
                c[i].type = linkShaderParams.constants[i].type;

                u32 len = rsl::strlen(linkShaderParams.constants[i].name);
                c[i].name = (char8*)memory_alloc(len + 1);

                memcpy(c[i].name, linkShaderParams.constants[i].name, len);
                c[i].name[len] = '\0';
            }

            add_cmd(cmd);

            return resource_slot;
        }

        //-------------------------------------------------------------------------
        void release_resource(u32 resourceTarget)
        {
            RenderCommand cmd;

            cmd.command_type = CommandType::RELEASE_RESOURCE;
            cmd.release_resource.resource_index = resourceTarget;

            add_cmd(cmd);
        }

        //-------------------------------------------------------------------------
        void clear(u32 clearStateTarget)
        {
            RenderCommand cmd;

            cmd.command_type = CommandType::CLEAR;
            cmd.clear.clear_state = clearStateTarget;

            add_cmd(cmd);
        }

        //-------------------------------------------------------------------------
        void renderer_draw(u32 vertexCount, u32 startVertex, PrimitiveTopology primitiveTopology)
        {
            RenderCommand cmd;

            cmd.command_type = CommandType::DRAW;
            cmd.draw.vertex_count = vertexCount;
            cmd.draw.start_vertex = startVertex;
            cmd.draw.primitive_topology = primitiveTopology;

            add_cmd(cmd);
        }

        //-------------------------------------------------------------------------
        void renderer_draw_indexed(u32 indexCount, u32 startIndex, u32 baseVertex, PrimitiveTopology primitiveTopology)
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
        void renderer_draw_indexed_instanced(u32 instanceCount, u32 startInstance, u32 indexCount, u32 startIndex, u32 baseVertex, PrimitiveTopology primitiveTopology)
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
        void set_raster_state(u32 rasterStateTarget)
        {
            RenderCommand cmd;

            cmd.command_type = CommandType::SET_RASTER_STATE;
            cmd.set_raster_state.raster_state = rasterStateTarget;

            add_cmd(cmd);
        }

        //-------------------------------------------------------------------------
        void set_render_targets(u32* colorTargets, u32 numColorTargets, u32 depthTarget, u32 arrayIndex)
        {
            RenderCommand cmd;

            cmd.command_type = CommandType::SET_RENDER_TARGETS;
            cmd.set_render_target.num_color = numColorTargets;
            memcpy(&cmd.set_render_target.color, colorTargets, numColorTargets * sizeof(u32));
            cmd.set_render_target.depth = depthTarget;
            cmd.set_render_target.array_index = arrayIndex;

            add_cmd(cmd);
        }

        //-------------------------------------------------------------------------
        void set_render_targets(u32 colorTarget, u32 depthTarget)
        {
            RenderCommand cmd;

            cmd.command_type = CommandType::SET_RENDER_TARGETS;
            cmd.set_render_target.num_color = is_valid(colorTarget) ? 1 : 0;
            cmd.set_render_target.color[0] = colorTarget;
            cmd.set_render_target.depth = depthTarget;
            cmd.set_render_target.array_index = 0;

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
        void set_input_layout(u32 inputLayoutTarget)
        {
            RenderCommand cmd;

            cmd.command_type = CommandType::SET_INPUT_LAYOUT;
            cmd.set_input_layout.input_layout = inputLayoutTarget;

            add_cmd(cmd);
        }

        //-------------------------------------------------------------------------
        void set_vertex_buffer(u32 vertexBufferTarget, u32 startSlot, u32 stride, u32 offset)
        {
            u32* vertex_buffer_target = &vertexBufferTarget;
            const u32* stride_target = &stride;
            const u32* offset_target = &offset;

            set_vertex_buffers(vertex_buffer_target, 1, startSlot, stride_target, offset_target);
        }
        //-------------------------------------------------------------------------
        void set_vertex_buffers(u32* vertexBufferTargets, u32 numBuffers, u32 startSlot, const u32* strides, const u32* offsets)
        {
            RenderCommand cmd;

            cmd.command_type = CommandType::SET_VERTEX_BUFFER;

            cmd.set_vertex_buffer.start_slot = startSlot;
            cmd.set_vertex_buffer.num_buffers = numBuffers;

            cmd.set_vertex_buffer.buffer_indices = (u32*)memory_alloc(sizeof(u32) * numBuffers);
            cmd.set_vertex_buffer.strides = (u32*)memory_alloc(sizeof(u32) * numBuffers);
            cmd.set_vertex_buffer.offsets = (u32*)memory_alloc(sizeof(u32) * numBuffers);

            for (u32 i = 0; i < numBuffers; ++i)
            {
                cmd.set_vertex_buffer.buffer_indices[i] = vertexBufferTargets[i];
                cmd.set_vertex_buffer.strides[i] = strides[i];
                cmd.set_vertex_buffer.offsets[i] = offsets[i];
            }

            add_cmd(cmd);
        }
        //-------------------------------------------------------------------------
        void set_index_buffer(u32 indexBufferTarget, u32 format, u32 offset)
        {
            RenderCommand cmd;

            cmd.command_type = CommandType::SET_INDEX_BUFFER;
            cmd.set_index_buffer.buffer_index = indexBufferTarget;
            cmd.set_index_buffer.format = format;
            cmd.set_index_buffer.offset = offset;

            add_cmd(cmd);
        }

        //-------------------------------------------------------------------------
        void set_shader(u32 shaderTarget, ShaderType shaderType)
        {
            RenderCommand cmd;

            cmd.command_type = CommandType::SET_SHADER;

            cmd.set_shader.shader_index = shaderTarget;
            cmd.set_shader.shader_type = shaderType;

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