#pragma once

#include "rex_engine/diagnostics/error.h"
#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/engine/invalid_object.h"
#include "rex_engine/engine/globals.h"

#include "rex_engine/gfx/core/renderer_output_window_user_data.h"

#include "rex_engine/gfx/core/graphics_engine_type.h"

#include "rex_engine/gfx/system/command_allocator.h"
#include "rex_engine/gfx/system/command_queue.h"


#include "rex_engine/gfx/system/render_context.h"
#include "rex_engine/gfx/system/compute_context.h"

#include "rex_engine/gfx/core/info.h"

#include "rex_engine/gfx/system/graphics_engine.h"
#include "rex_engine/gfx/rendering/renderer.h"

#include "rex_engine/gfx/resources/sampler_2d.h"
#include "rex_engine/gfx/resources/raster_state.h"

#include "rex_std/bonus/utility.h"

#include "rex_engine/pooling/scoped_pooled_object.h"



// This is the interface of the main application with the graphics engine

#include "rex_engine/gfx/core/texture_format.h"
#include "rex_engine/gfx/core/index_buffer_format.h"
#include "rex_engine/gfx/system/shader_platform.h"

#include "rex_engine/gfx/resources/resource.h"
#include "rex_engine/gfx/resources/render_target.h"
#include "rex_engine/gfx/resources/texture_2d.h"
#include "rex_engine/gfx/resources/sampler_2d.h"
#include "rex_engine/gfx/system/command_queue.h"
#include "rex_engine/gfx/system/command_allocator.h"
#include "rex_engine/gfx/system/debug_interface.h"
#include "rex_engine/gfx/system/swapchain.h"
#include "rex_engine/gfx/rendering/renderer.h"
#include "rex_engine/gfx/resources/backbuffer_render_target.h"
#include "rex_engine/gfx/resources/vertex_buffer.h"
#include "rex_engine/gfx/resources/index_buffer.h"
#include "rex_engine/gfx/resources/root_signature.h"
#include "rex_engine/gfx/resources/pipeline_state.h"
#include "rex_engine/gfx/resources/raster_state.h"
#include "rex_engine/gfx/resources/constant_buffer.h"
#include "rex_engine/gfx/resources/input_layout.h"
#include "rex_engine/gfx/resources/upload_buffer.h"
#include "rex_engine/gfx/resources/depth_stencil_buffer.h"
#include "rex_engine/gfx/resources/unordered_access_buffer.h"
#include "rex_engine/gfx/resources/animated_sprite.h"
#include "rex_engine/gfx/materials/material.h"
#include "rex_engine/gfx/system/shader_pipeline.h"
#include "rex_engine/gfx/shader_reflection/shader_signature.h"
#include "rex_engine/gfx/shader_reflection/shader_param_declaration.h"

#include "rex_engine/gfx/core/renderer_output_window_user_data.h"
#include "rex_engine/pooling/scoped_pooled_object.h"
#include "rex_engine/gfx/core/graphics_engine_type.h"

#include "rex_engine/gfx/system/compute_context.h"
#include "rex_engine/gfx/system/render_context.h"
#include "rex_engine/gfx/system/command_queue.h"
#include "rex_engine/gfx/core/gpu_description.h"
#include "rex_engine/gfx/system/render_engine.h"
#include "rex_engine/gfx/system/compute_engine.h"
#include "rex_engine/gfx/resources/clear_state.h"
#include "rex_engine/gfx/system/resource_view_type.h"
#include "rex_engine/gfx/system/view_heap.h"
#include "rex_engine/gfx/system/swapchain.h"
#include "rex_engine/gfx/system/resource_state_tracker.h"

 

namespace rex
{
  struct GpuDescription;

  namespace gfx
  {
    class GALBase;
    class PipelineState;
    class LegacyRenderer;

    namespace gal
    {
			// Create and initialize the graphics engine, rendering is possible from here on out
			Error init(globals::GlobalUniquePtr<GALBase> galInterface);
			GALBase* instance();
			// Shutdown the graphics engine, no rendering support from here on out
			void shutdown();
		}

    // The following functions allow you to return a few common resource items
    enum class CommonRasterState
    {
      DefaultNoDepth,
      DefaultDepth
    };
    enum class CommonSampler
    {
      Default2D
    };

    DEFINE_YES_NO_ENUM(IsShaderVisible);
    struct Info;
    struct MaterialDesc;

    // GAL stands for Graphics Abstraction Layer
    // It is the layer between the program and the graphics systems
    class GALBase
    {
    public:
      GALBase(const OutputWindowUserData& userData);
      virtual ~GALBase() = default;

      // --------------------------------
      // Application API
      // --------------------------------
      // This is a virtual function as some of the common items are stored in the base class,
      // however they can only be initialized by the derived class.
      // Because virtual functions cannot be called from the constructor
      // we split initialization up in 2 phases
      void init();

      // Render a single frame by going over all the renderers
      void render();

      // Flush all commands
      void flush();

      // Resize swapchain buffers
      void resize_backbuffers(s32 newWidth, s32 newHeight);

      // --------------------------------
      // Getters
      // --------------------------------
      // Return the max number of frames we can render at once
      s32 max_frames_in_flight() const;

      // Return the command queue of the render engine
      // This is usually needed by graphics plugins (like ImGui)
      CommandQueue* render_command_queue();

      // Returns the format of the swapchain's buffers
      TextureFormat swapchain_format() const;

      // Return basic info about the graphics hardware of the current machine
      virtual const Info& info() const = 0;

      // --------------------------------
      // Configuration
      // --------------------------------
      virtual ShaderPlatform shader_platform() const = 0;

      // --------------------------------
      // Backend Systems
      // --------------------------------
      virtual rsl::unique_ptr<CommandQueue> create_command_queue(GraphicsEngineType type) = 0;
      virtual rsl::unique_ptr<Swapchain> create_swapchain(void* apiDevice, s32 bufferCount, void* primaryDisplayHandle) = 0;
      virtual rsl::unique_ptr<CommandAllocator> create_command_allocator(GraphicsEngineType type) = 0;

      // --------------------------------
      // Resource creation
      // --------------------------------
      virtual rsl::unique_ptr<RenderTarget> create_render_target(s32 width, s32 height, TextureFormat format) = 0;
      virtual rsl::unique_ptr<VertexBuffer> create_vertex_buffer(s32 numVertices, s32 vertexSize, const void* data = nullptr) = 0;
      virtual rsl::unique_ptr<IndexBuffer> create_index_buffer(s32 numIndices, IndexBufferFormat format, const void* data = nullptr) = 0;
      virtual rsl::unique_ptr<RootSignature> create_root_signature(const rsl::vector<ShaderParameterDeclaration>& parameters) = 0;
      virtual rsl::unique_ptr<PipelineState> create_pso(const PipelineStateDesc& desc) = 0;
      virtual rsl::unique_ptr<Texture2D> create_texture2d(s32 width, s32 height, TextureFormat format, const void* data = nullptr) = 0;
      virtual rsl::unique_ptr<DepthStencilBuffer> create_depth_stencil_buffer(s32 width, s32 height, TextureFormat format, const ClearStateDesc& clearStateDesc) = 0;
      virtual rsl::unique_ptr<ConstantBuffer> create_constant_buffer(rsl::memory_size size, rsl::string_view debugName = "Constant Buffer") = 0;
      virtual rsl::unique_ptr<InputLayout> create_input_layout(const InputLayoutDesc& desc) = 0;
      virtual rsl::unique_ptr<Shader> create_vertex_shader(rsl::string_view sourceCode, rsl::string_view shaderName = "") = 0;
      virtual rsl::unique_ptr<Shader> create_vertex_shader(const memory::Blob& byteBlob) = 0;
      virtual rsl::unique_ptr<Shader> create_pixel_shader(rsl::string_view sourceCode, rsl::string_view shaderName = "") = 0;
      virtual rsl::unique_ptr<Shader> create_pixel_shader(const memory::Blob& byteBlob) = 0;
      virtual rsl::unique_ptr<UploadBuffer> create_upload_buffer(rsl::memory_size size) = 0;
      virtual rsl::unique_ptr<Material> create_material(const MaterialDesc& matDesc) = 0;
      virtual rsl::unique_ptr<Sampler2D> create_sampler2d(const SamplerDesc& desc) = 0;
      virtual rsl::unique_ptr<UnorderedAccessBuffer> create_unordered_access_buffer(rsl::memory_size size, const void* data = nullptr) = 0;
      
      AnimatedSprite* create_animated_sprite(Flipbook* animations);

      // --------------------------------
      // View creation
      // --------------------------------
      // Create a shader resource view pointing to a render target
      virtual ResourceView* create_srv(const RenderTarget* rt) = 0;

      // --------------------------------
      // Contexts
      // --------------------------------
      // Create a new context which is used for rendering to render targets
      ScopedGraphicsContext<RenderContext> new_render_ctx(PipelineState* pso = nullptr, rsl::string_view eventName = "");
      // Create a new context which is used for computing data on the gpu
      ScopedGraphicsContext<ComputeContext> new_compute_ctx(PipelineState* pso = nullptr, rsl::string_view eventName = "");

      // --------------------------------
      // Render Pipeline
      // --------------------------------
			// Return the width of the render target of the swapchain
			s32 back_buffer_width() const;
			// Return the height of the render target of the swapchain
			s32 back_buffer_height() const;
      // Return the current render target of the swapchain
      BackBufferRenderTarget* backbuffer_rendertarget();
      // Construct a new renderer and add it to the list of renderers we should call
      template <typename T, typename ... Args>
      T* add_renderer(Args&& ... args)
      {
        rsl::unique_ptr<T> renderer = rsl::make_unique<T>(rsl::forward<Args>(args)...);

        T* result = renderer.get();
        m_renderers.emplace_back(rsl::move(renderer));

        return result;
      }

      // --------------------------------
      // GPU RESOURCE QUERYING
      // --------------------------------
      const ResourceView* try_get_gpu_views(const rsl::vector<const ResourceView*>& views) const;
      const ResourceView* try_get_gpu_view(const ResourceView* cpuView) const;
      const ResourceView* notify_views_on_gpu(const rsl::vector<const ResourceView*>& views, rsl::unique_ptr<ResourceView> gpuView);

      RasterStateDesc common_raster_state(CommonRasterState type);
      Sampler2D* common_sampler(CommonSampler type);

    protected:
      virtual void api_init() = 0;

      // Initialize the various sub engines
      virtual rsl::unique_ptr<RenderEngine> init_render_engine(ResourceStateTracker* resourceStateTracker) = 0;
      virtual rsl::unique_ptr<ComputeEngine> init_compute_engine(ResourceStateTracker* resourceStateTracker) = 0;

      // Allocate the debug interface.
      // This is done in the API specific level but we need to store the pointer in the base as it needs to get destroyed last
      // That's why we need this function
      virtual rsl::unique_ptr<DebugInterface> allocate_debug_interface() = 0;
      // Initialize the resource heap which allocates all gpu resources
      virtual void init_resource_heap() = 0;
      // Allocate a new descriptor heap of a given type
      virtual rsl::unique_ptr<ViewHeap> allocate_view_heap(ResourceViewType descHeapType, IsShaderVisible isShaderVisible) = 0;

      // Returns a specific descriptor heap based on type
      // Return the view heap, accessible from the CPU
      ViewHeap* cpu_view_heap(ResourceViewType descHeapType);
      // Return the view heap, accessible from the GPU
      ViewHeap* gpu_view_heap(ResourceViewType descHeapType);

      // Returns the debug interface
      // This is sometimes needed in API specific code
      DebugInterface* debug_interface();

    private:
      using ViewHeapPool = rsl::unordered_map<ResourceViewType, rsl::unique_ptr<ViewHeap>>;

      // --------------------------------
      // Initialization
      // --------------------------------

      void init_debug_interface();
      // Initialize the swapchain which is used for presenting to the main window
      void init_swapchain();
      // Initialize the sub engine, bringing them up and ready, to be used in the graphics pipeline
      void init_sub_engines();
      // Initialize the descriptor heaps which keep track of all descriptors to various resources
      void init_view_heaps();
      void init_view_heap(ViewHeapPool& descHeapPool, ResourceViewType descHeapType, IsShaderVisible isShaderVisible);

      void init_common_resources();

      // --------------------------------
      // Rendering
      // --------------------------------

      // Prepare a new frame by incrementing the frame index and clearing the backbuffer
      void new_frame();
      // Present the new frame to the main window
      void present();
      // Finish off the last frame
      void end_frame();

      // Create a context reset structure, filling it in with all the data it needs to reset a context
      ContextResetData create_context_reset_data(PipelineState* pso);
      void log_info(const Info& info);

    private:
      rsl::unique_ptr<DebugInterface> m_debug_interface;  // Used to determine if we have any leaking resource on shutdown
      rsl::unique_ptr<Swapchain> m_swapchain;             // The swapchain is responsible for swapping the backbuffer with the front buffer
      rsl::unique_ptr<RenderEngine> m_render_engine;      // The render engine is the high level graphics engine responsible for queueing render commands
      rsl::unique_ptr<ComputeEngine> m_compute_engine;    // The render engine is the high level graphics engine responsible for queueing compute commands
      void* m_primary_display_handle;                     // The display handle to render to (HWND on Windows)
      s32 m_max_frames_in_flight;                         // The maximum number of we can have in flight for rendering.
      s32 m_frame_idx;                                    // The current frame index
      ViewHeapPool m_cpu_descriptor_heap_pool;            // Pool of descriptor heaps per type
      ViewHeapPool m_shader_visible_descriptor_heap_pool; // Pool of descriptor heaps per type
      ResourceStateTracker m_resource_state_tracker;      // The global tracker of resource states
      rsl::unique_ptr<BackBufferRenderTarget> m_backbuffer_render_target;

      rsl::unordered_map<u64, rsl::unique_ptr<ResourceView>> m_resources_on_gpu;                                    // Holds a map of all resources already present on the GPU and their views
      rsl::vector<rsl::unique_ptr<LegacyRenderer>> m_renderers;                                                           // Holds all the renderers
      rsl::array<RasterStateDesc, rsl::enum_refl::enum_count<CommonRasterState>()> m_common_raster_states;          // Holds a list of common raster states
      rsl::array<rsl::unique_ptr<Sampler2D>, rsl::enum_refl::enum_count<CommonSampler>()> m_common_samplers;        // Holds a list of common samplers
    };
  }
}