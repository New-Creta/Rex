#pragma once

#include "rex_engine/engine/types.h"

namespace rex
{
    struct Viewport;
    struct ScissorRect;

    namespace renderer
    {
        struct OutputWindowUserData;
        
        class ResourceSlot;

        enum class PrimitiveTopology;
        enum class IndexBufferFormat;

        namespace commands
        {
            struct CreateClearStateCommandDesc;
            struct CreateRasterStateCommandDesc;
            struct CreateBufferCommandDesc;
            struct CreateIndexBufferCommandDesc;
            struct CreateConstantBufferViewCommandDesc;
            struct CreatePipelineStateCommandDesc;
            struct CreateInputLayoutCommandDesc;
            struct AttachCommittedResourceToFrameCommandDesc;
            struct BeginDrawCommandDesc;
            struct EndDrawCommandDesc;
            struct NewFrameCommandDesc;
            struct EndFrameCommandDesc;
            struct CompileShaderCommandDesc;
            struct LinkShaderCommandDesc;
            struct LoadShaderCommandDesc;
            struct ClearCommandDesc;
            struct DrawCommandDesc;
            struct DrawIndexedCommandDesc;
            struct DrawIndexedInstancedCommandDesc;
            struct DrawInstanceCommandDesc;
            struct ReleaseResourceCommandDesc;
            struct UpdateCommittedResourceCommandDesc;
        }

        namespace backend
        {
            bool                flush_command_queue();

            bool                initialize(const OutputWindowUserData& userData, s32 maxFrameResources, const ResourceSlot& fbColorTargetSlot, const ResourceSlot& bbColorTargetSlot, const ResourceSlot& depthTargetSlot);
            void                shutdown();

            // Platform specific implementation, implements these function
            const ResourceSlot* active_frame();
            const ResourceSlot* frame_at_index(s32 idx);
            s32                 max_frames_in_flight();

            bool                create_clear_state(const commands::CreateClearStateCommandDesc& cs, const ResourceSlot& resourceSlot);
            bool                create_raster_state(const commands::CreateRasterStateCommandDesc& rs, const ResourceSlot& resourceSlot);
            bool                create_input_layout(const commands::CreateInputLayoutCommandDesc& cil, const ResourceSlot& resourceSlot);
            bool                create_vertex_buffer(const commands::CreateBufferCommandDesc& cb, const ResourceSlot& resourceSlot);
            bool                create_index_buffer(const commands::CreateBufferCommandDesc& cb, const ResourceSlot& resourceSlot);
            bool                create_constant_buffer_view(const commands::CreateConstantBufferViewCommandDesc& cb, const ResourceSlot& resourceSlot);
            bool                create_pipeline_state_object(const commands::CreatePipelineStateCommandDesc& cps, const ResourceSlot& resourceSlot);
            bool                create_frame_resource(const ResourceSlot& resourceSlot);
            bool                attach_committed_resource_to_frame(const commands::AttachCommittedResourceToFrameCommandDesc& acrd, const ResourceSlot& resourceSlot);

            bool                load_shader(const commands::LoadShaderCommandDesc& ls, const ResourceSlot& resourceSlot);
            bool                link_shader(const commands::LinkShaderCommandDesc& ls, const ResourceSlot& resourceSlot);
            bool                compile_shader(const commands::CompileShaderCommandDesc& cs, const ResourceSlot& resourceSlot);

            void                update_committed_resource(const commands::UpdateCommittedResourceCommandDesc& updateConstantBuffer, const ResourceSlot& resourceSlot);

            void                wait_for_active_frame();

            bool                release_resource(const ResourceSlot& resourceSlot);

            bool                prepare_user_initialization();
            bool                finish_user_initialization();

            void                clear(const ResourceSlot& clearStateIndex);

            void                draw(s32 vertexCount, s32 startVertex);
            void                draw_indexed(s32 indexCount, s32 startIndex, s32 baseVertex);
            void                draw_instanced(s32 vertexCount, s32 instanceCount, s32 startVertex, s32 startInstance);
            void                draw_indexed_instanced(s32 instanceCount, s32 startInstance, s32 indexCount, s32 startIndex, s32 baseVertex);

            bool                set_render_targets(const ResourceSlot* const colorTargets, s32 numColorTargets, const ResourceSlot& depthTarget);
            bool                set_input_layout(const ResourceSlot& inputLayoutTarget);
            bool                set_viewport(const Viewport& viewport);
            bool                set_scissor_rect(const ScissorRect& rect);
            bool                set_vertex_buffers(const ResourceSlot* vertexBufferTargets, s32 numBuffers, s32 startSlot, const s32* strides, const s32* offsets);
            bool                set_index_buffer(const ResourceSlot& indexBufferTarget, IndexBufferFormat format, s32 offset);
            bool                set_constant_buffer_view(const ResourceSlot& constantBufferTarget, s32 location);
            bool                set_shader(const ResourceSlot& shaderTarget);
            bool                set_raster_state(const ResourceSlot& rasterStateTarget);
            bool                set_pipeline_state_object(const ResourceSlot& psoTarget);
            bool                set_primitive_topology(PrimitiveTopology primitiveTopology);

            // functions to synchronize the render and main threads
            bool                new_frame();
            bool                end_frame();

            bool                begin_draw();
            bool                end_draw();

            bool                present();
        }       // namespace backend
    }
}