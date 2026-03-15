#include "rex_engine/gfx/rendering/frame_buffer.h"

#include "rex_engine/diagnostics/assert.h"

#include "rex_engine/gfx/system/render_context.h"
#include "rex_engine/gfx/system/swapchain_framebuffer_handle.h"
#include "rex_engine/gfx/graphics.h"

namespace rex
{
	namespace gfx
	{
		FrameBufferAttachmentDesc::FrameBufferAttachmentDesc(SwapchainFrameBufferHandle)
			: m_use_swapchain(true)
			, m_depth_stencil_buffer(nullptr)
		{}

		FrameBufferAttachmentDesc::FrameBufferAttachmentDesc(s32 width, s32 height, TextureFormat format, const ClearStateDesc& clearStateDesc)
			: m_width(width)
			, m_height(height)
			, m_format(format)
			, m_clear_state_desc(clearStateDesc)
			, m_depth_stencil_buffer(nullptr)
		{}

		FrameBufferAttachmentDesc::FrameBufferAttachmentDesc(RenderTargetBase* renderTarget)
			: m_render_target(renderTarget)
			, m_width(renderTarget->width())
			, m_height(renderTarget->height())
			, m_format(TextureFormat::Unorm4Srgb)
			, m_depth_stencil_buffer(nullptr)
		{}

		bool FrameBufferAttachmentDesc::use_swapchain() const
		{
			return m_use_swapchain;
		}
		RenderTargetBase* FrameBufferAttachmentDesc::render_target() const 
		{
			return m_render_target;
		}
		DepthStencilBuffer* FrameBufferAttachmentDesc::depth_stencil_buffer() const 
		{
			return m_depth_stencil_buffer;
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

		ColorBufferAttachment::ColorBufferAttachment(RenderTargetBase* resourceView)
			: m_render_target(resourceView)
		{}
		FrameBufferAttachmentDesc::FrameBufferAttachmentDesc(DepthStencilBuffer* depthStencilBuffer)
		{
			m_depth_stencil_buffer = depthStencilBuffer;
			m_width = m_depth_stencil_buffer->width();
			m_height = m_depth_stencil_buffer->height();
			m_format = m_depth_stencil_buffer->format();
			m_clear_state_desc = m_depth_stencil_buffer->clear_state();
		}


		ColorBufferAttachment::ColorBufferAttachment(SwapchainFrameBufferHandle)
			: m_use_swapchain(true)
		{}

		RenderTargetBase* ColorBufferAttachment::render_target()
		{
			return m_render_target;
		}
		bool ColorBufferAttachment::use_swapchain() const
		{
			return m_use_swapchain;
		}

		FrameBuffer::FrameBuffer(const FrameBufferDesc& desc)
			: m_depth_stencil_buffer(nullptr)
		{
			for (auto& attachment_desc : desc)
			{
				if (attachment_desc.use_swapchain())
				{
					REX_STATIC_TODO("This can be removed and replaced with BackBufferSwapChain")
					m_attachments.emplace_back(swapchain_frame_buffer_handle());
				}
				else if (attachment_desc.render_target())
				{
					m_attachments.emplace_back(attachment_desc.render_target());
				}
				else
				{
					REX_STATIC_TODO("Requires cleanup");
					if (is_depth_format(attachment_desc.format()))
					{
						//REX_ASSERT_X(m_depth_stencil_buffer == nullptr, "You can only create 1 depth stencil buffer per framebuffer");
						m_depth_stencil_buffer = attachment_desc.depth_stencil_buffer(); // gfx::gal::instance()->create_depth_stencil_buffer(attachment_desc.width(), attachment_desc.height(), attachment_desc.format(), attachment_desc.clear_state());
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
			REX_STATIC_TODO("Requires cleanup");
			if (m_attachments.front().use_swapchain())
			{
				// As the swapchain's back buffer gets cleared on a new frame, we don't have to clear it here
				//ctx->set_render_target(gfx::gal::instance()->backbuffer_rendertarget(), m_depth_stencil_buffer.get());
				//if (m_depth_stencil_buffer.get())
				//{
				//	ctx->clear_depth_stencil_target(m_depth_stencil_buffer.get());
				//}
			}
			else
			{
				ctx->set_render_target(m_attachments.front().render_target(), m_depth_stencil_buffer);
				//ctx->clear_render_target(m_attachments.front().render_target(), m_depth_stencil_buffer.get());
			}
		}

		RenderTargetBase* FrameBuffer::render_target(s32 idx)
		{
			return m_attachments[idx].render_target();
		}
	}
}