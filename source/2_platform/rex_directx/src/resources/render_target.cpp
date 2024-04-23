#include "rex_directx/resources/render_target.h"
#include "rex_directx/resources/texture.h"

namespace rex
{
  namespace renderer
  {
    RenderTarget::RenderTarget()
        : m_textures(rsl::Size(AttachmentPoint::NumAttachmentPoints))
        , m_size(0, 0)
    {
    }

    // Attach a texture to the render target.
    // The texture will be copied into the texture array.
    void RenderTarget::attach_texture(AttachmentPoint attachmentPoint, const rsl::shared_ptr<Texture>& texture)
    {
      m_textures[attachmentPoint] = texture;

      if(texture && texture->d3d_resource())
      {
        auto desc = texture->d3d_resource_desc();

        m_size.x = static_cast<u32>(desc.Width);
        m_size.y = static_cast<u32>(desc.Height);
      }
    }

    // Resize all of the textures associated with the render target.
    void RenderTarget::resize(DirectX::XMUINT2 size)
    {
      m_size = size;
      for(auto texture: m_textures)
      {
        if(texture)
        {
          texture->resize(m_size.x, m_size.y);
        }
      }
    }

    void RenderTarget::resize(u32 width, u32 height)
    {
      resize(DirectX::XMUINT2(width, height));
    }

    DirectX::XMUINT2 RenderTarget::size() const
    {
      return m_size;
    }

    u32 RenderTarget::width() const
    {
      return m_size.x;
    }

    u32 RenderTarget::height() const
    {
      return m_size.y;
    }

    D3D12_VIEWPORT RenderTarget::viewport(DirectX::XMFLOAT2 scale, DirectX::XMFLOAT2 bias, float minDepth, float maxDepth) const
    {
      u64 width = 0;
      u32 height  = 0;

      for(int i = AttachmentPoint::Color0; i <= AttachmentPoint::Color7; ++i)
      {
        auto texture = m_textures[i];
        if(texture)
        {
          auto desc = texture->d3d_resource_desc();

          width     = (rsl::max)(width, desc.Width);
          height    = (rsl::max)(height, desc.Height);
        }
      }

      D3D12_VIEWPORT viewport = {
          (width * bias.x),   // Top Left X
          (height * bias.y),  // Top Left Y
          (width * scale.x),  // Width
          (height * scale.y), // Height
          minDepth,           // Min Depth
          maxDepth            // Max Depth
      };

      return viewport;
    }

    const rsl::shared_ptr<Texture>& RenderTarget::texture(AttachmentPoint attachmentPoint) const
    {
      return m_textures[attachmentPoint];
    }

    // Get a list of the textures attached to the render target.
    // This method is primarily used by the command_list when binding the
    // render target to the output merger stage of the rendering pipeline.
    const rsl::vector<rsl::shared_ptr<Texture>>& RenderTarget::textures() const
    {
      return m_textures;
    }

    D3D12_RT_FORMAT_ARRAY RenderTarget::render_target_formats() const
    {
      D3D12_RT_FORMAT_ARRAY rtv_formats = {};

      for(int i = AttachmentPoint::Color0; i <= AttachmentPoint::Color7; ++i)
      {
        auto texture = m_textures[i];
        if(texture)
        {
          rtv_formats.RTFormats[rtv_formats.NumRenderTargets++] = texture->d3d_resource_desc().Format;
        }
      }

      return rtv_formats;
    }

    DXGI_FORMAT RenderTarget::depth_stencil_format() const
    {
      DXGI_FORMAT dsv_format = DXGI_FORMAT_UNKNOWN;

      auto depth_stencil_texture = m_textures[AttachmentPoint::DepthStencil];
      if(depth_stencil_texture)
      {
        dsv_format = depth_stencil_texture->d3d_resource_desc().Format;
      }

      return dsv_format;
    }

    DXGI_SAMPLE_DESC RenderTarget::sample_desc() const
    {
      DXGI_SAMPLE_DESC sample_desc = {1, 0};
      for(int i = AttachmentPoint::Color0; i <= AttachmentPoint::Color7; ++i)
      {
        auto texture = m_textures[i];
        if(texture)
        {
          sample_desc = texture->d3d_resource_desc().SampleDesc;
          break;
        }
      }

      return sample_desc;
    }

    void RenderTarget::reset()
    {
      m_textures = render_target_list(rsl::Size(AttachmentPoint::NumAttachmentPoints));
    }
  } // namespace renderer
} // namespace rex