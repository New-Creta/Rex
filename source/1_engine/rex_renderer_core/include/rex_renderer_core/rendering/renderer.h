#pragma once

#include "rex_engine/engine/types.h"
#include "rex_std/bonus/utility/yes_no.h"

namespace rex
{
  struct Viewport;
  struct ScissorRect;
  struct DefaultTargetsInfo;
  struct DefaultDepthInfo;

  namespace rhi
  {
    class ResourceSlot;
  }

  namespace globals
  {
    const DefaultTargetsInfo& default_targets_info();
    const DefaultDepthInfo& default_depth_info();
  } // namespace globals

  namespace renderer
  {
    DEFINE_YES_NO_ENUM(FlushCommands);

    struct Info;
    struct OutputWindowUserData;

    class Mesh;

    enum class ShaderPlatform;
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
    } // namespace commands

    bool initialize(const OutputWindowUserData& userData, s32 maxCommands, s32 maxFrameResources);
    void render();
    void shutdown();

    struct RenderItem* add_mesh(const Mesh* mesh);



    // general accessors
    const Info& info();
    ShaderPlatform shader_platform();

    bool is_y_up();
    bool is_depth_0_to_1();

    // public-api will buffer all commands for dispatch on dedicated thread
    //const ResourceSlot* active_frame();
    //const ResourceSlot* frame_at_index(s32 idx);
    s32 max_frames_in_flight();

    //ResourceSlot create_clear_state(commands::CreateClearStateCommandDesc&& desc);
    //ResourceSlot create_raster_state(commands::CreateRasterStateCommandDesc&& desc);
    //ResourceSlot create_input_layout(commands::CreateInputLayoutCommandDesc&& desc);
    //ResourceSlot create_vertex_buffer(commands::CreateBufferCommandDesc&& createBufferParams);
    //ResourceSlot create_index_buffer(commands::CreateBufferCommandDesc&& createBufferParams);
    //ResourceSlot create_constant_buffer_view(commands::CreateConstantBufferViewCommandDesc&& createBufferParams);
    //ResourceSlot create_pipeline_state_object(commands::CreatePipelineStateCommandDesc&& createPipelineStateParams);
    //ResourceSlot create_frame_resource();
    //ResourceSlot attach_committed_resource_to_frame(commands::AttachCommittedResourceToFrameCommandDesc&& attachCommittedResourceParams);

    //ResourceSlot load_shader(commands::LoadShaderCommandDesc&& loadShaderParams);
    //ResourceSlot link_shader(commands::LinkShaderCommandDesc&& linkShaderParams);
    //ResourceSlot compile_shader(commands::CompileShaderCommandDesc&& compileShaderParams);

    //bool update_committed_resource(commands::UpdateCommittedResourceCommandDesc&& updateConstantBufferParams, const ResourceSlot& constantBufferTarget);

    //void wait_for_active_frame();

    //bool release_resource(const ResourceSlot& resourceTarget);

    bool prepare_user_initialization();
    bool finish_user_initialization();

    //bool clear(const ResourceSlot& clearStateTarget);

    //bool renderer_draw(s32 vertexCount, s32 startVertex);
    //bool renderer_draw_indexed(s32 indexCount, s32 startIndex, s32 baseVertex);
    //bool renderer_draw_indexed_instanced(s32 instanceCount, s32 startInstance, s32 indexCount, s32 startIndex, s32 baseVertex);
    //bool renderer_draw_instanced(s32 vertexCount, s32 instanceCount, s32 startVertex, s32 startInstance);

    //bool set_raster_state(const ResourceSlot& rasterStateTarget);
    //bool set_render_targets(const ResourceSlot* colorTargets, s32 numColorTargets, const ResourceSlot& depthTarget);
    //bool set_render_targets(const ResourceSlot& colorTarget, const ResourceSlot& depthTarget);
    //bool set_viewport(const Viewport& vp);
    //bool set_scissor_rect(const ScissorRect& sr);
    //bool set_input_layout(const ResourceSlot& inputLayoutTarget);
    //bool set_vertex_buffer(const ResourceSlot& vertexBufferTarget, s32 startSlot, s32 stride, s32 offset);
    //bool set_vertex_buffers(const ResourceSlot* vertexBufferTargets, s32 numBuffers, s32 startSlot, const s32* strides, const s32* offsets);
    //bool set_index_buffer(const ResourceSlot& indexBufferTarget, IndexBufferFormat format, s32 offset);
    //bool set_shader(const ResourceSlot& shaderTarget);
    //bool set_pipeline_state_object(const ResourceSlot& psoTarget);
    //bool set_constant_buffer_view(const ResourceSlot& constantBufferTarget, s32 location);
    //bool set_primitive_topology(PrimitiveTopology primitiveTopology);

    void set_viewport(const Viewport& viewport);
    void set_scissor_rect(const ScissorRect& rect);

    void set_shader(const rhi::ResourceSlot& slot);
    void set_pso(const rhi::ResourceSlot& slot);

    bool new_frame();
    bool end_frame(FlushCommands flush = FlushCommands::yes);

    bool begin_draw();
    bool end_draw();

    bool present();
    bool flush();
  } // namespace renderer
} // namespace rex