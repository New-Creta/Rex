#pragma once

#include "rex_std/vector.h"
#include "rex_engine/engine/types.h"
#include "rex_engine/gfx/resources/clear_state.h"
#include "rex_engine/gfx/system/swapchain_framebuffer_handle.h"
#include "rex_engine/gfx/core/texture_format.h"

namespace rex
{
	namespace gfx
	{
		class RenderTargetBase;
		class RenderContext;
		class DepthStencilBuffer;

		// A frame buffer attachment can be provided in 3 ways
		// - define the width, height and format of the attachment, causing it to be created at frame buffer construction
		// - a pre allocated resource, perhaps shared with other frame buffers
		// - a handle indicating that the swapchain's backbuffer should be used
		class FrameBufferAttachmentDesc
		{
		public:
			FrameBufferAttachmentDesc(SwapchainFrameBufferHandle);
			FrameBufferAttachmentDesc(s32 width, s32 height, TextureFormat format, const ClearStateDesc& clearStateDesc);
			FrameBufferAttachmentDesc(RenderTargetBase* renderTarget);
			FrameBufferAttachmentDesc(DepthStencilBuffer* depthStencilBuffer);

			// Getters
			bool use_swapchain() const;
			RenderTargetBase* render_target() const;
			DepthStencilBuffer* depth_stencil_buffer() const;
			s32 width() const;
			s32 height() const;
			TextureFormat format() const;
			const ClearStateDesc& clear_state() const;

		private:
			s32 m_width = -1;
			s32 m_height = -1;
			TextureFormat m_format = TextureFormat::Unknown;
			bool m_use_swapchain = false;
			RenderTargetBase* m_render_target = nullptr;
			DepthStencilBuffer* m_depth_stencil_buffer = nullptr;
			ClearStateDesc m_clear_state_desc;
		};

		class ColorBufferAttachment
		{
		public:
			ColorBufferAttachment(RenderTargetBase* resourceView);
			ColorBufferAttachment(SwapchainFrameBufferHandle);

			// Returns the render target of the attachment
			RenderTargetBase* render_target();
			// Returns true if the attachment indicates that the swapchain's backbuffer should be used as render target
			bool use_swapchain() const;

		private:
			RenderTargetBase* m_render_target = nullptr;
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

			RenderTargetBase* render_target(s32 idx);

		private:
			constexpr static s32 s_max_num_rendertargets = 8;
			rsl::vector<ColorBufferAttachment> m_attachments;
			DepthStencilBuffer* m_depth_stencil_buffer;
		};
	}
}