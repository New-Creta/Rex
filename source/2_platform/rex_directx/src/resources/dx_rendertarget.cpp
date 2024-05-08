#include "rex_directx/resources/dx_rendertarget.h"

namespace rex
{
  namespace resources
  {
    RenderTarget::RenderTarget(const wrl::ComPtr<ID3D12Resource>& resource, s32 width, s32 height)
      : m_resource(resource)
      , m_width(width)
      , m_height(height)
    {

    }

  }
}