#include "rex_renderer_core/rendering/frame_buffer.h"

#include "rex_engine/diagnostics/assert.h"
#include "rex_renderer_core/gfx/rhi.h"
#include "rex_renderer_core/gfx/render_context.h"
#include "rex_renderer_core/gfx/graphics.h"

namespace rex
{
	namespace gfx
	{
		SwapchainFrameBufferHandle swapchain_frame_buffer_handle()
		{
			return SwapchainFrameBufferHandle{};
		}

		FrameBufferAttachmentDesc::FrameBufferAttachmentDesc(SwapchainFrameBufferHandle)
			: m_use_swapchain(true)
		{}

		FrameBufferAttachmentDesc::FrameBufferAttachmentDesc(s32 width, s32 height, TextureFormat format, const ClearStateDesc& clearStateDesc)
			: m_width(width)
			, m_height(height)
			, m_format(format)
			, m_clear_state_desc(clearStateDesc)
		{}

		FrameBufferAttachmentDesc::FrameBufferAttachmentDesc(RenderTarget* renderTarget)
			: m_render_target(renderTarget)
		{}

		bool FrameBufferAttachmentDesc::use_swapchain() const
		{
			return m_use_swapchain;
		}
		RenderTarget* FrameBufferAttachmentDesc::render_target() const
		{
			return m_render_target;
		}
		s32 FrameBufferAttachmentDesc::width() const
		{
			return m_width;
		}
		s32 FrameBufferAttachmentDesc::height() const
		{
			return m_height;
		}
		TextureFormat FrameBufferAttachmentDesc::format() const
		{
			return m_format;
		}
		const ClearStateDesc& FrameBufferAttachmentDesc::clear_state() const
		{
			return m_clear_state_desc;
		}

		ColorBufferAttachment::ColorBufferAttachment(RenderTarget* resourceView)
			: m_render_target(resourceView)
		{}
		FrameBufferAttachmentDesc::FrameBufferAttachmentDesc(DepthStencilBuffer* /*depthStencilBuffer*/)
		{
			REX_ASSERT("To implement");
		}


		ColorBufferAttachment::ColorBufferAttachment(SwapchainFrameBufferHandle)
			: m_use_swapchain(true)
		{}

		RenderTarget* ColorBufferAttachment::render_target() const
		{
			return m_render_target;
		}
		bool ColorBufferAttachment::use_swapchain() const
		{
			return m_use_swapchain;
		}

		FrameBuffer::FrameBuffer(const FrameBufferDesc& desc)
		{
			for (const auto& attachment_desc : desc)
			{
				if (attachment_desc.use_swapchain())
				{
					m_attachments.emplace_back(swapchain_frame_buffer_handle());
				}
				else if (attachment_desc.render_target())
				{
					m_attachments.emplace_back(attachment_desc.render_target());
				}
				else
				{
					if (is_depth_format(attachment_desc.format()))
					{
						REX_ASSERT_X(m_depth_stencil_buffer == nullptr, "You can only create 1 depth stencil buffer per framebuffer");
						m_depth_stencil_buffer = rhi::create_depth_stencil_buffer(attachment_desc.width(), attachment_desc.height(), attachment_desc.format(), attachment_desc.clear_state());
					}
					else
					{
						REX_ASSERT("To implement");
					}
				}
			}

			REX_ASSERT_X(m_attachments.size() < s_max_num_rendertargets, "Using more than {} render targets for a frame buffer, this is not allowed", s_max_num_rendertargets);
		}

		void FrameBuffer::bind_to(RenderContext* ctx)
		{
			if (m_attachments.front().use_swapchain())
			{
				// As the swapchain's back buffer gets cleared on a new frame, we don't have to clear it here
				ctx->set_render_target(swapchain_rt(), m_depth_stencil_buffer.get());
				if (m_depth_stencil_buffer.get())
				{
					ctx->clear_depth_stencil_target(m_depth_stencil_buffer.get());
				}
			}
			else
			{
				ctx->set_render_target(m_attachments.front().render_target(), m_depth_stencil_buffer.get());
				ctx->clear_render_target(m_attachments.front().render_target(), m_depth_stencil_buffer.get());
			}
		}
	}
}