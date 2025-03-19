#pragma once

#include "rex_engine/gfx/system/command_allocator_pool.h"
#include "rex_engine/gfx/system/command_queue.h"
#include "rex_engine/gfx/core/graphics_engine_type.h"
#include "rex_engine/gfx/system/graphics_context.h"
#include "rex_engine/gfx/system/graphics_context_pool.h"
#include "rex_engine/gfx/system/resource_state_tracker.h"

#include "rex_engine/engine/types.h"
#include "rex_engine/engine/invalid_object.h"
#include "rex_std/bonus/utility.h"

namespace rex
{
  namespace gfx
  {
    class ViewHeap;
    class WaitForFinish;
    class PipelineState;
    class RenderTarget;
  }

  // #TODO: Remaining cleanup of development/Pokemon -> main merge. ID: GRAPHICS
  // #TODO: Remaining cleanup of development/Pokemon -> main merge. ID: OBJECT WITH DESTRUCTION CALLBACK

  namespace gfx
  {
    struct ContextResetData
    {
      // The pipeline state to reset the context with, this can be nullptr
      PipelineState* pso;
      
      // The descriptor heap holding the descriptors for all resources that are needed to perform a queued draw command
      // the descriptors in this heap are accessible by shaders
      ViewHeap* shader_visible_srv_desc_heap;

      // The descriptor heap holding the descriptors for all samplers that are needed to perform a queued draw command
      // the descriptors in this heap are accessible by shaders
      ViewHeap* shader_visible_sampler_desc_heap;

      // The render target pointing to the current back buffer in the swapchain
      RenderTarget* current_backbuffer_rt;
    };

    // A graphics engine is an engine responsible for their own respective field of the graphics pipeline.
    // Each graphics engine owns the command queue used for their part of the graphics pipeline.
    // They also hold a list of active and idle contexts which are used to queue up gpu commands
    // Lastly they also hold a command allocator pool which is used to allocate new command allocators from
    // and pool them up when they're not used.
    class GraphicsEngine
    {
    public:
      GraphicsEngine(GpuEngine* gpuEngine, GraphicsEngineType type, ResourceStateTracker* globalResourceStateTracker);
      virtual ~GraphicsEngine();

      // Executes the context and returns the fence value that'll be set when all commands are executed
      ObjectWithDestructionCallback<SyncInfo> execute_context(GraphicsContext* context, WaitForFinish waitForFinish);
      
      // Get a new context object from the engine, using an idle one or creating a new one if no idle one is found
      ObjectWithDestructionCallback<GraphicsContext> new_context(const ContextResetData& resetData, rsl::string_view eventName = "");

      // Halt gpu commands from being executed until the sync info object is triggered
      void stall(SyncInfo& syncInfo);

      // Return the command queue owned by the graphics engine, this is needed for the swapchain
      CommandQueue* command_queue();
      // Return the command type of the engine
      GraphicsEngineType type() const;
      // Return the resource tracker of this engine
      ResourceStateTracker* resource_state_tracker();

      GpuEngine* gpu_engine();

      // Fully initialize the engine, allocating all required resources etc
      virtual void init() = 0;
      // Prepare the engine for executing a new frame
      virtual void new_frame() = 0;
      // End the engine for executing the last frame
      virtual void end_frame() = 0;

    protected:
      // Flush all commands on the gpu and halt the current thread untill all commands are executed
      void flush();
      // Allocates a new API specific graphics context
      virtual rsl::unique_ptr<GraphicsContext> allocate_new_context(CommandAllocator* alloc) = 0;

    private:
      // Request a new allocator from the command allocator pool
      ObjectWithDestructionCallback<PooledAllocator> request_allocator();

    private:
      GpuEngine* m_gpu_engine;
      rsl::unique_ptr<CommandQueue> m_command_queue;        // the command queue to submit gpu commands
      GraphicsContextPool m_context_pool;                   // the pool for graphics contexts of this type of graphics engine
      gfx::CommandAllocatorPool m_command_allocator_pool;   // the pool for command allocators
      ResourceStateTracker m_resource_state_tracker;        // the resource state tracker of this engine
    };
  }
}