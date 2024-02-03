#include "rex_renderer_core/renderer.h"

#include "rex_engine/engine/defines.h"
#include "rex_engine/ring_buffer.h"
#include "rex_renderer_core/commands/attach_committed_resource_to_frame_cmd.h"
#include "rex_renderer_core/commands/begin_draw_cmd.h"
#include "rex_renderer_core/commands/clear_cmd.h"
#include "rex_renderer_core/commands/compile_shader_cmd.h"
#include "rex_renderer_core/commands/create_buffer_cmd.h"
#include "rex_renderer_core/commands/create_clear_state_cmd.h"
#include "rex_renderer_core/commands/create_constant_buffer_cmd.h"
#include "rex_renderer_core/commands/create_frame_resource_cmd.h"
#include "rex_renderer_core/commands/create_index_buffer_cmd.h"
#include "rex_renderer_core/commands/create_input_layout_cmd.h"
#include "rex_renderer_core/commands/create_pipeline_state_cmd.h"
#include "rex_renderer_core/commands/create_raster_state_cmd.h"
#include "rex_renderer_core/commands/create_vertex_buffer_cmd.h"
#include "rex_renderer_core/commands/draw_cmd.h"
#include "rex_renderer_core/commands/draw_indexed_cmd.h"
#include "rex_renderer_core/commands/draw_indexed_instanced_cmd.h"
#include "rex_renderer_core/commands/draw_instanced_cmd.h"
#include "rex_renderer_core/commands/end_draw_cmd.h"
#include "rex_renderer_core/commands/end_frame_cmd.h"
#include "rex_renderer_core/commands/finish_user_initialization_cmd.h"
#include "rex_renderer_core/commands/link_shader_cmd.h"
#include "rex_renderer_core/commands/load_shader_cmd.h"
#include "rex_renderer_core/commands/new_frame_cmd.h"
#include "rex_renderer_core/commands/prepare_user_initialization_cmd.h"
#include "rex_renderer_core/commands/present_cmd.h"
#include "rex_renderer_core/commands/release_resource_cmd.h"
#include "rex_renderer_core/commands/render_cmd.h"
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
#include "rex_renderer_core/commands/update_committed_resource_cmd.h"
#include "rex_renderer_core/default_depth_info.h"
#include "rex_renderer_core/default_targets_info.h"
#include "rex_renderer_core/index_buffer_format.h"
#include "rex_renderer_core/log.h"
#include "rex_renderer_core/primitive_topology.h"
#include "rex_renderer_core/renderer_backend.h"
#include "rex_renderer_core/resource_slots.h"
#include "rex_renderer_core/scissor_rect.h"
#include "rex_renderer_core/shader_platform.h"
#include "rex_renderer_core/shader_type.h"
#include "rex_renderer_core/viewport.h"
#include "rex_std/bonus/memory/stack_allocator.h"
#include "rex_std/bonus/utility/enum_reflection.h"

namespace rex
{
  namespace globals
  {
    DefaultDepthInfo g_default_depth_info;     // NOLINT(fuchsia-statically-constructed-objects, cppcoreguidelines-avoid-non-const-global-variables)
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
    using CommandAllocator = rsl::stack_allocator;
    using CommandList      = RingBuffer<RenderCommand*>;

    constexpr s32 cmd_allocator_size = static_cast<s32>((512_kib).size_in_bytes());

    struct Context
    {
      rsl::unique_ptr<ResourceSlots> slot_resources;
      rsl::unique_ptr<CommandAllocator> cmd_allocator;
      rsl::unique_ptr<CommandList> cmd_list;
    };

    rsl::unique_ptr<Context> g_ctx; // NOLINT(fuchsia-statically-constructed-objects, cppcoreguidelines-avoid-non-const-global-variables)

#if REX_SINGLE_THREADED
    bool process_render_command(renderer::RenderCommand* cmd)
    {
      return cmd->execute();
    };
#else
    bool process_render_command(renderer::RenderCommand* cmd)
    {
      return g_ctx->cmd_list->put(cmd);
    };
#endif

    //-------------------------------------------------------------------------
    template <typename TCommandType, typename... Args>
    TCommandType* create_new_command(Args&&... args)
    {
      TCommandType* cmd = static_cast<TCommandType*>(g_ctx->cmd_allocator->allocate(sizeof(TCommandType)));
      return new(cmd) TCommandType(rsl::forward<Args>(args)...);
    }

    //-------------------------------------------------------------------------
    template <typename TCommandType, typename... Args>
    TCommandType* create_new_command(Args&&... args, const ResourceSlot& slot)
    {
      TCommandType* cmd = static_cast<TCommandType*>(g_ctx->cmd_allocator->allocate(sizeof(TCommandType)));
      return new(cmd) TCommandType(rsl::forward<Args>(args)...);
    }

    namespace internal
    {
      //-------------------------------------------------------------------------
      template <typename TCommandType>
      ResourceSlot create_buffer(commands::CreateBufferCommandDesc&& createBufferParams)
      {
        ResourceSlot resource_slot = g_ctx->slot_resources->alloc_slot();

        TCommandType* cmd = create_new_command<TCommandType>(rsl::move(createBufferParams), resource_slot);

        if(!process_render_command(cmd))
        {
          REX_ERROR(LogRendererCore, "Unable to add/exec create buffer command");
          return ResourceSlot::make_invalid();
        }

        return resource_slot;
      }

      //-------------------------------------------------------------------------
      ResourceSlot create_constant_buffer_view(commands::CreateConstantBufferViewCommandDesc&& createBufferParams)
      {
        ResourceSlot resource_slot = g_ctx->slot_resources->alloc_slot();

        commands::CreateConstantBufferView* cmd = create_new_command<commands::CreateConstantBufferView>(rsl::move(createBufferParams), resource_slot);

        if(!process_render_command(cmd))
        {
          REX_ERROR(LogRendererCore, "Unable to add/exec create constant buffer command");
          return ResourceSlot::make_invalid();
        }

        return resource_slot;
      }

      //-------------------------------------------------------------------------
      bool flush(RingBuffer<RenderCommand*>* cmdList)
      {
        // Cache executed commands to destroy after we flushed
        rsl::vector<RenderCommand*> executed_commands;

        RenderCommand** cmd = cmdList->get();

        while(cmd)
        {
          if((*cmd)->execute() == false) // NOLINT(readability-simplify-boolean-expr)
          {
            REX_ERROR(LogRendererCore, "Failed to flush commands");
            return false;
          }

          executed_commands.push_back(*cmd);

          cmd = g_ctx->cmd_list->get();
        }

        // Manually call the destructor of each command
        // We will reset the stack allocator back to the beginning and override the existing memory
        // This will not call any dtors while this happens
        for(RenderCommand* executed_cmd: executed_commands)
        {
          executed_cmd->~RenderCommand();
        }

        return true;
      }
    } // namespace internal

    //-------------------------------------------------------------------------
    bool initialize(const OutputWindowUserData& userData, s32 maxCommands, s32 maxFrameResources)
    {
      g_ctx = rsl::make_unique<Context>();

      // We will initialize the allocated resource to 16 to pool can always grow when it requires more slots
      //      we use a limited amount of resources now so 16 is definitly large enough
      //      we can always tweak this value later as pass it as an argument to the initialize of the renderer.
      constexpr s32 initial_allocated_resource_slots = 16;

      g_ctx->slot_resources = rsl::make_unique<ResourceSlots>(initial_allocated_resource_slots);
      g_ctx->cmd_allocator  = rsl::make_unique<CommandAllocator>(cmd_allocator_size);
      g_ctx->cmd_list       = rsl::make_unique<CommandList>(maxCommands);

      globals::g_default_targets_info.front_buffer_color = g_ctx->slot_resources->alloc_slot();
      globals::g_default_targets_info.back_buffer_color  = g_ctx->slot_resources->alloc_slot();
      globals::g_default_targets_info.depth_buffer       = g_ctx->slot_resources->alloc_slot();

      return backend::initialize(userData, maxFrameResources, globals::g_default_targets_info.front_buffer_color, globals::g_default_targets_info.back_buffer_color, globals::g_default_targets_info.depth_buffer);
    }

    //-------------------------------------------------------------------------
    void shutdown()
    {
      flush();

      // These are tracked separetly, so we have to release them manually
      globals::g_default_targets_info.back_buffer_color.release();
      globals::g_default_targets_info.front_buffer_color.release();
      globals::g_default_targets_info.depth_buffer.release();

      g_ctx.reset();

      backend::shutdown();
    }

    //-------------------------------------------------------------------------
    const ResourceSlot* active_frame()
    {
      return backend::active_frame();
    }

    //-------------------------------------------------------------------------
    const ResourceSlot* frame_at_index(s32 idx)
    {
      return backend::frame_at_index(idx);
    }

    //-------------------------------------------------------------------------
    s32 max_frames_in_flight()
    {
      return backend::max_frames_in_flight();
    }

    //-------------------------------------------------------------------------
    ResourceSlot create_clear_state(commands::CreateClearStateCommandDesc&& desc)
    {
      ResourceSlot resource_slot = g_ctx->slot_resources->alloc_slot();

      commands::CreateClearState* cmd = create_new_command<commands::CreateClearState>(rsl::move(desc), resource_slot);

      if(!process_render_command(cmd))
      {
        REX_ERROR(LogRendererCore, "Unable to add/exec create clear state command");
        return ResourceSlot::make_invalid();
      }

      return resource_slot;
    }

    //-------------------------------------------------------------------------
    ResourceSlot create_raster_state(commands::CreateRasterStateCommandDesc&& desc)
    {
      ResourceSlot resource_slot = g_ctx->slot_resources->alloc_slot();

      commands::CreateRasterState* cmd = create_new_command<commands::CreateRasterState>(rsl::move(desc), resource_slot);

      if(!process_render_command(cmd))
      {
        REX_ERROR(LogRendererCore, "Unable to add/exec create raster state command");
        return ResourceSlot::make_invalid();
      }

      return resource_slot;
    }

    //-------------------------------------------------------------------------
    ResourceSlot create_input_layout(commands::CreateInputLayoutCommandDesc&& desc)
    {
      ResourceSlot resource_slot = g_ctx->slot_resources->alloc_slot();

      commands::CreateInputLayout* cmd = create_new_command<commands::CreateInputLayout>(rsl::move(desc), resource_slot);

      if(!process_render_command(cmd))
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
    ResourceSlot create_constant_buffer_view(commands::CreateConstantBufferViewCommandDesc&& createBufferParams)
    {
      return internal::create_constant_buffer_view(rsl::move(createBufferParams));
    }

    //-------------------------------------------------------------------------
    ResourceSlot create_pipeline_state_object(commands::CreatePipelineStateCommandDesc&& createPipelineStateParams)
    {
      ResourceSlot resource_slot = g_ctx->slot_resources->alloc_slot();

      commands::CreatePipelineState* cmd = create_new_command<commands::CreatePipelineState>(rsl::move(createPipelineStateParams), resource_slot);

      if(!process_render_command(cmd))
      {
        REX_ERROR(LogRendererCore, "Unable to add/exec create pipeline state object command");
        return ResourceSlot::make_invalid();
      }

      return resource_slot;
    }

    //-------------------------------------------------------------------------
    ResourceSlot create_frame_resource()
    {
      ResourceSlot resource_slot = g_ctx->slot_resources->alloc_slot();

      commands::CreateFrameResource* cmd = create_new_command<commands::CreateFrameResource>(commands::CreateFrameResourceCommandDesc(), resource_slot);

      if(!process_render_command(cmd))
      {
        REX_ERROR(LogRendererCore, "Unable to add/exec create frame resource command");
        return ResourceSlot::make_invalid();
      }

      return resource_slot;
    }

    //-------------------------------------------------------------------------
    ResourceSlot attach_committed_resource_to_frame(commands::AttachCommittedResourceToFrameCommandDesc&& attachCommittedResourceParams)
    {
      ResourceSlot resource_slot = g_ctx->slot_resources->alloc_slot();

      commands::AttachCommittedResourceToFrame* cmd = create_new_command<commands::AttachCommittedResourceToFrame>(rsl::move(attachCommittedResourceParams), resource_slot);

      if(!process_render_command(cmd))
      {
        REX_ERROR(LogRendererCore, "Unable to add/exec attach committed resource to frame command");
        return ResourceSlot::make_invalid();
      }

      return resource_slot;
    }

    //-------------------------------------------------------------------------
    ResourceSlot load_shader(commands::LoadShaderCommandDesc&& loadShaderParams)
    {
      ResourceSlot resource_slot = g_ctx->slot_resources->alloc_slot();

      commands::LoadShader* cmd = create_new_command<commands::LoadShader>(rsl::move(loadShaderParams), resource_slot);

      if(!process_render_command(cmd))
      {
        REX_ERROR(LogRendererCore, "Unable to add/exec load shader command");
        return ResourceSlot::make_invalid();
      }

      return resource_slot;
    }

    //-------------------------------------------------------------------------
    ResourceSlot link_shader(commands::LinkShaderCommandDesc&& linkShaderParams)
    {
      ResourceSlot resource_slot = g_ctx->slot_resources->alloc_slot();

      commands::LinkShader* cmd = create_new_command<commands::LinkShader>(rsl::move(linkShaderParams), resource_slot);

      if(!process_render_command(cmd))
      {
        REX_ERROR(LogRendererCore, "Unable to add/exec link shader command");
        return ResourceSlot::make_invalid();
      }

      return resource_slot;
    }

    //-------------------------------------------------------------------------
    ResourceSlot compile_shader(commands::CompileShaderCommandDesc&& compileShaderParams)
    {
      ResourceSlot resource_slot = g_ctx->slot_resources->alloc_slot();

      commands::CompileShader* cmd = create_new_command<commands::CompileShader>(rsl::move(compileShaderParams), resource_slot);

      if(!process_render_command(cmd))
      {
        REX_ERROR(LogRendererCore, "Unable to add/exec compile shader command");
        return ResourceSlot::make_invalid();
      }

      return resource_slot;
    }

    //-------------------------------------------------------------------------
    bool update_committed_resource(commands::UpdateCommittedResourceCommandDesc&& updateConstantBufferParams, const ResourceSlot& constantBufferTarget)
    {
      commands::UpdateCommittedResource* cmd = create_new_command<commands::UpdateCommittedResource>(rsl::move(updateConstantBufferParams), constantBufferTarget);

      return process_render_command(cmd);
    }

    //-------------------------------------------------------------------------
    void wait_for_active_frame()
    {
      backend::wait_for_active_frame();
    }

    //-------------------------------------------------------------------------
    bool release_resource(const ResourceSlot& resourceTarget)
    {
      return backend::release_resource(resourceTarget);
    }

    //-------------------------------------------------------------------------
    bool prepare_user_initialization()
    {
      commands::PrepareUserInitialization* cmd = create_new_command<commands::PrepareUserInitialization>(commands::PrepareUserInitializationCommandDesc {});

      return process_render_command(cmd);
    }

    //-------------------------------------------------------------------------
    bool finish_user_initialization()
    {
      commands::FinishUserInitialization* cmd = create_new_command<commands::FinishUserInitialization>(commands::FinishUserInitializationCommandDesc {});

      return process_render_command(cmd);
    }

    //-------------------------------------------------------------------------
    bool clear(const ResourceSlot& clearStateTarget)
    {
      commands::Clear* cmd = create_new_command<commands::Clear>(commands::ClearCommandDesc(), clearStateTarget);

      return process_render_command(cmd);
    }

    //-------------------------------------------------------------------------
    bool renderer_draw(s32 vertexCount, s32 startVertex)
    {
      commands::Draw* cmd = create_new_command<commands::Draw>(commands::DrawCommandDesc {vertexCount, startVertex});

      return process_render_command(cmd);
    }

    //-------------------------------------------------------------------------
    bool renderer_draw_indexed(s32 indexCount, s32 startIndex, s32 baseVertex)
    {
      commands::DrawIndexed* cmd = create_new_command<commands::DrawIndexed>(commands::DrawIndexedCommandDesc {indexCount, startIndex, baseVertex});

      return process_render_command(cmd);
    }

    //-------------------------------------------------------------------------
    bool renderer_draw_indexed_instanced(s32 instanceCount, s32 startInstance, s32 indexCount, s32 startIndex, s32 baseVertex)
    {
      commands::DrawIndexedInstanced* cmd = create_new_command<commands::DrawIndexedInstanced>(commands::DrawIndexedInstancedCommandDesc {instanceCount, startInstance, indexCount, startIndex, baseVertex});

      return process_render_command(cmd);
    }

    //-------------------------------------------------------------------------
    bool renderer_draw_instanced(s32 vertexCount, s32 instanceCount, s32 startVertex, s32 startInstance)
    {
      commands::DrawInstanced* cmd = create_new_command<commands::DrawInstanced>(commands::DrawInstanceCommandDesc {instanceCount, startInstance, vertexCount, startVertex});

      return process_render_command(cmd);
    }

    //-------------------------------------------------------------------------
    bool set_raster_state(const ResourceSlot& rasterStateTarget)
    {
      commands::SetRasterState* cmd = create_new_command<commands::SetRasterState>(commands::SetRasterStateCommandDesc {}, rasterStateTarget);

      return process_render_command(cmd);
    }

    //-------------------------------------------------------------------------
    bool set_render_targets(const ResourceSlot* colorTargets, s32 numColorTargets, const ResourceSlot& depthTarget)
    {
      commands::SetRenderTargetCommandDesc set_render_target_command_desc;
      set_render_target_command_desc.num_color = numColorTargets;
      for(s32 i = 0; i < numColorTargets; ++i)
      {
        set_render_target_command_desc.color[i] = colorTargets[i];
      }
      set_render_target_command_desc.depth = depthTarget;

      commands::SetRenderTarget* cmd = create_new_command<commands::SetRenderTarget>(rsl::move(set_render_target_command_desc));

      return process_render_command(cmd);
    }

    //-------------------------------------------------------------------------
    bool set_render_targets(const ResourceSlot& colorTarget, const ResourceSlot& depthTarget)
    {
      commands::SetRenderTargetCommandDesc set_render_target_command_desc;
      set_render_target_command_desc.num_color = colorTarget.is_valid() ? 1 : 0;
      set_render_target_command_desc.color[0]  = colorTarget;
      set_render_target_command_desc.depth     = depthTarget;

      commands::SetRenderTarget* cmd = create_new_command<commands::SetRenderTarget>(rsl::move(set_render_target_command_desc));

      return process_render_command(cmd);
    }

    //-------------------------------------------------------------------------
    bool set_viewport(const Viewport& vp)
    {
      commands::SetViewport* cmd = create_new_command<commands::SetViewport>(commands::SetViewportCommandDesc {vp});

      return process_render_command(cmd);
    }

    //-------------------------------------------------------------------------
    bool set_scissor_rect(const ScissorRect& sr)
    {
      commands::SetScissorRect* cmd = create_new_command<commands::SetScissorRect>(commands::SetScissorRectCommandDesc {sr});

      return process_render_command(cmd);
    }

    //-------------------------------------------------------------------------
    bool set_input_layout(const ResourceSlot& inputLayoutTarget)
    {
      commands::SetInputLayout* cmd = create_new_command<commands::SetInputLayout>(commands::SetInputLayoutCommandDesc {}, inputLayoutTarget);

      return process_render_command(cmd);
    }

    //-------------------------------------------------------------------------
    bool set_vertex_buffer(const ResourceSlot& vertexBufferTarget, s32 startSlot, s32 stride, s32 offset)
    {
      const ResourceSlot* vertex_buffer_target = &vertexBufferTarget;
      const s32* stride_target                 = &stride;
      const s32* offset_target                 = &offset;

      return set_vertex_buffers(vertex_buffer_target, 1, startSlot, stride_target, offset_target);
    }
    //-------------------------------------------------------------------------
    bool set_vertex_buffers(const ResourceSlot* vertexBufferTargets, s32 numBuffers, s32 startSlot, const s32* strides, const s32* offsets)
    {
      commands::SetVertexBufferCommandDesc set_vertex_buffer_command_desc;

      set_vertex_buffer_command_desc.start_slot = startSlot;

      set_vertex_buffer_command_desc.vertex_buffer_targets = rsl::vector<ResourceSlot>(rsl::Size(numBuffers));
      set_vertex_buffer_command_desc.strides               = rsl::vector<s32>(rsl::Size(numBuffers));
      set_vertex_buffer_command_desc.offsets               = rsl::vector<s32>(rsl::Size(numBuffers));

      for(s32 i = 0; i < numBuffers; ++i)
      {
        set_vertex_buffer_command_desc.vertex_buffer_targets[i] = vertexBufferTargets[i];
        set_vertex_buffer_command_desc.strides[i]               = strides[i];
        set_vertex_buffer_command_desc.offsets[i]               = offsets[i];
      }

      commands::SetVertexBuffer* cmd = create_new_command<commands::SetVertexBuffer>(rsl::move(set_vertex_buffer_command_desc));

      return process_render_command(cmd);
    }
    //-------------------------------------------------------------------------
    bool set_index_buffer(const ResourceSlot& indexBufferTarget, IndexBufferFormat format, s32 offset)
    {
      commands::SetIndexBuffer* cmd = create_new_command<commands::SetIndexBuffer>(commands::SetIndexBufferCommandDesc {format, offset}, indexBufferTarget);

      return process_render_command(cmd);
    }

    //-------------------------------------------------------------------------
    bool set_shader(const ResourceSlot& shaderTarget)
    {
      commands::SetShader* cmd = create_new_command<commands::SetShader>(commands::SetShaderCommandDesc {}, shaderTarget);

      return process_render_command(cmd);
    }

    //-------------------------------------------------------------------------
    bool set_pipeline_state_object(const ResourceSlot& psoTarget)
    {
      commands::SetPipelineState* cmd = create_new_command<commands::SetPipelineState>(commands::SetPipelineStateCommandDesc {}, psoTarget);

      return process_render_command(cmd);
    }

    //-------------------------------------------------------------------------
    bool set_constant_buffer_view(const ResourceSlot& constantBufferTarget, s32 location)
    {
      commands::SetConstantBufferView* cmd = create_new_command<commands::SetConstantBufferView>(commands::SetConstantBufferViewCommandDesc {location}, constantBufferTarget);

      return process_render_command(cmd);
    }

    //-------------------------------------------------------------------------
    bool set_primitive_topology(PrimitiveTopology primitiveTopology)
    {
      commands::SetPrimitiveTopology* cmd = create_new_command<commands::SetPrimitiveTopology>(commands::SetPrimitiveTopologyCommandDesc {primitiveTopology});

      return process_render_command(cmd);
    }

    //-------------------------------------------------------------------------
    bool new_frame()
    {
      commands::NewFrame* cmd = create_new_command<commands::NewFrame>(commands::NewFrameCommandDesc {});

      return process_render_command(cmd);
    }

    //-------------------------------------------------------------------------
    bool end_frame(FlushCommands flush)
    {
      commands::EndFrame* cmd = create_new_command<commands::EndFrame>(commands::EndFrameCommandDesc {});

      bool result = process_render_command(cmd);

      if(result)
      {
        if(flush)
        {
          result = internal::flush(g_ctx->cmd_list.get());

          g_ctx->cmd_allocator->reset();
        }

        return result;
      }

      REX_ERROR(LogRendererCore, "Unable to EndFrame, commands are not flushed!");

      return result;
    }

    //-------------------------------------------------------------------------
    bool begin_draw()
    {
      commands::BeginDraw* cmd = create_new_command<commands::BeginDraw>(commands::BeginDrawCommandDesc {});

      return process_render_command(cmd);
    }

    //-------------------------------------------------------------------------
    bool end_draw()
    {
      commands::EndDraw* cmd = create_new_command<commands::EndDraw>(commands::EndDrawCommandDesc {});

      return process_render_command(cmd);
    }

    //-------------------------------------------------------------------------
    bool present()
    {
      commands::Present* cmd = create_new_command<commands::Present>(commands::PresentCommandDesc {&globals::g_default_targets_info.front_buffer_color, &globals::g_default_targets_info.back_buffer_color});

      return process_render_command(cmd);
    }

    //-------------------------------------------------------------------------
    bool flush()
    {
      return internal::flush(g_ctx->cmd_list.get());
    }
  } // namespace renderer
} // namespace rex