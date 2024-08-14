#pragma once

#include "rex_renderer_core/resources/pipeline_state.h"
#include "rex_renderer_core/resources/root_signature.h"
#include "rex_renderer_core/gfx/graphics.h"
#include "rex_renderer_core/system/shader_parameters_store.h"
#include "rex_renderer_core/resources/depth_stencil_buffer.h"
#include "rex_renderer_core/resources/clear_state.h"

#include "rex_engine/text_processing/json.h"

#include "rex_std/string.h"
#include "rex_std/memory.h"
#include "rex_std/optional.h"
#include "rex_engine/engine/types.h"

namespace rex
{
	namespace gfx
	{
		class Texture2D;
		class Sampler2D;

		// A drawlist holds data needed to draw a single element to the screen
		struct DrawList
		{
			VertexBuffer* vb;   // The vertex buffer of the object
			IndexBuffer* ib;	// The index buffer of the object
			ConstantBuffer* per_instance_cb; // The constant buffer holding per instance data
		};

		// In the future we'll provide a way to specify that the
		struct FrameBufferAttachmentDesc
		{
			FrameBufferAttachmentDesc(bool useSwapchain)
				: use_swapchain(useSwapchain)
			{}

			FrameBufferAttachmentDesc(s32 width, s32 height, TextureFormat format)
				: width(width)
				, height(height)
				, format(format)
			{}
			
			FrameBufferAttachmentDesc(ResourceView* renderTarget)
				: resource_view(renderTarget)
			{}

			s32 width;
			s32 height;
			TextureFormat format;
			bool use_swapchain;
			ResourceView* resource_view;
		};

		struct FrameBufferAttachment
		{
			FrameBufferAttachment(ResourceView* resourceView)
				: resource_view(resourceView)
				, use_swapchain(false)
			{}

			FrameBufferAttachment(bool useSwapchain)
				: resource_view(nullptr)
				, use_swapchain(useSwapchain)
			{}

			ResourceView* resource_view;
			bool use_swapchain;
		};

		struct FrameBufferDesc
		{
			rsl::vector<FrameBufferAttachmentDesc> attachment_descs;


			// The total attachments for the frame buffer for a render pass
			// is the combination of the predefined render targets (which could be render targets of other passes)
			// as well as the attachments which get turned into render targets at render pass construction
			//rsl::vector<RenderTarget*> render_targets; // The render targets for the render pass
			//rsl::vector<FrameBufferAttachment> render_target_attachments; // Additional render target attachments for non-yet created resources
		};

		class FrameBuffer
		{
		public:
			FrameBuffer(const FrameBufferDesc& desc);

			void bind_to(RenderContext* ctx, ClearState* clearState = nullptr);

		private:
			rsl::vector<FrameBufferAttachment> m_attachments;
			rsl::vector<RenderTarget*> m_render_targets;
			rsl::unique_ptr<DepthStencilBuffer> m_depth_stencil_buffer;
		};

		// A descriptor on how to initialize the render pass
		struct RenderPassDesc
		{
			rsl::string_view name;			// The name of the render pass, mainly for debugging purposes
			PipelineStateDesc pso_desc;		// The pso desc that'll be used to create the pso for this render pass
			FrameBufferDesc framebuffer_desc; // The framebuffer desc that'll be used to create the framebuffer for this render pass
			rsl::optional<ClearStateDesc> clear_state_desc;
		};
		
		// A render pass acts like the "material" for the render pipeline
		// It holds a set of data that needs to be tied to the render pipeline
		// as well as the logic on how to use the render pipeline to draw
		// elements to the screen
		class RenderPass
		{
		public:
			RenderPass(const RenderPassDesc& desc);

			// Bind the resources of the render pass to a render context
			void bind_to(RenderContext* ctx);
			
			// Set renderpass parameter to a new resource
			void set(rsl::string_view name, ConstantBuffer* constantBuffer);
			void set(rsl::string_view name, Texture2D* texture);
			void set(rsl::string_view name, Sampler2D* sampler);

			// Return the slot of a renderpass parameter
			s32 slot(rsl::string_view name) const;

			// Set the blend factor of the material
			void set_blend_factor(const BlendFactor& blendFactor);

		private:
			rsl::tiny_stack_string m_name;								// The name of the renderpass
			rsl::unique_ptr<ShaderParametersStore> m_parameters_store;	// The store holding all renderpass parameters
			rsl::unique_ptr<FrameBuffer> m_framebuffer;
			rsl::unique_ptr<ClearState> m_clear_state;

			// The blend factor of the material, belongs to the output merger
			BlendFactor m_blend_factor;

			rsl::unique_ptr<PipelineState> m_pso;						// The pipeline state of the renderpass
		};

		rsl::optional<RenderPassDesc> load_from_json_path(rsl::string_view filepath);
	}
}