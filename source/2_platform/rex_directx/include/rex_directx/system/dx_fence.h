#pragma once

#include "rex_directx/utility/dx_util.h"
#include "rex_engine/engine/types.h"
#include "rex_renderer_core/system/fence.h"

namespace rex
{
  namespace gfx
  {
    class DxFence : public Fence
    {
    public:
      explicit DxFence(const wrl::ComPtr<ID3D12Fence>& fence);

      // Return the wrapped fence object
      ID3D12Fence* get() const;

    private:
      wrl::ComPtr<ID3D12Fence> m_fence;
    };
  } // namespace gfx
} // namespace rex