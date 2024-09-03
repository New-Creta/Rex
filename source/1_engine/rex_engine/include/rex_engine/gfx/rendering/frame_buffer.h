#pragma once

#include "rex_std/vector.h"
#include "rex_engine/engine/types.h"
#include "rex_engine/gfx/resources/clear_state.h"
#include "rex_engine/gfx/core/texture_format.h"

namespace rex
{
	namespace gfx
	{
		class RenderTarget;
		class RenderContext;
		class DepthStencilBuffer;

		// An empty structure, it's just to indicate that the swapchain's backbuffer should be used as color buffer
		struct SwapchainFrameBufferHandle {};
		// This function returns a new swapchain frame buffer handle
		SwapchainFrameBufferHandle swapchain_frame_buffer_handle();

		// A frame buffer attachment can be provided in 3 ways
		// - define the width, height and format of the attachment, causing it to be created at frame buffer construction
		// - a pre allocated resource, perhaps shared with other frame buffers
		// - a handle indicating that the swapchain's backbuffer should be used
		class FrameBufferAttachmentDesc
		{
		public:
			FrameBufferAttachmentDesc(SwapchainFrameBufferHandle);
			FrameBufferAttachmentDesc(s32 width, s32 height, TextureFormat format, const ClearStateDesc& clearStateDesc);
			FrameBufferAttachmentDesc(RenderTarget* renderTarget);
			FrameBufferAttachmentDesc(DepthStencilBuffer* depthStencilBuffer);

			// Getters
			bool use_swapchain() const;
			RenderTarget* render_target() const;
			s32 width() const;
			s32 height() const;
			TextureFormat format() const;
			const ClearStateDesc& clear_state() const;

		private:
			s32 m_width = -1;
			s32 m_height = -1;
			TextureFormat m_format = TextureFormat::Unknown;
			bool m_use_swapchain = false;
			RenderTarget* m_render_target = nullptr;
			ClearStateDesc m_clear_state_desc;
		};

		class ColorBufferAttachment
		{
		public:
			ColorBufferAttachment(RenderTarget* resourceView);
			ColorBufferAttachment(SwapchainFrameBufferHandle);

			// Returns the render target of the attachment
			RenderTarget* render_target() const;
			// Returns true if the attachment indicates that the swapchain's backbuffer should be used as render target
			bool use_swapchain() const;

		private:
			RenderTarget* m_render_target = nullptr;
			bool m_use_swapchain = false;
		};

		using FrameBufferDesc = rsl::vector<FrameBufferAttachmentDesc>;

		// A framebuffer can have up to 8 render targets and 1 depth stencil buffer
		// The render targets and depth stencil buffer can either be created at frame buffer construction
		// or if an existing resource is passed in through the desc, that'll be used as the attachment
		class FrameBuffer
		{
		public:
			FrameBuffer(const FrameBufferDesc& desc);

			// Bind the framebuffer to the render context
			void bind_to(RenderContext* ctx);

		private:
			constexpr static s32 s_max_num_rendertargets = 8;
			rsl::vector<ColorBufferAttachment> m_attachments;
			rsl::unique_ptr<DepthStencilBuffer> m_depth_stencil_buffer;
		};
	}
}