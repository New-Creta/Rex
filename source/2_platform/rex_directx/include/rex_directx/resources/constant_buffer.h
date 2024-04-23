#pragma once

#include "rex_directx/directx_util.h"
#include "rex_directx/resources/buffer.h"

namespace rex
{
  namespace renderer
  {
    class ConstantBuffer : public Buffer
    {
    public:
      RESOURCE_CLASS_TYPE(ConstantBuffer);

      size_t size_in_bytes() const;

    protected:
      ConstantBuffer(Device& device, wrl::ComPtr<ID3D12Resource> resource);
      ~ConstantBuffer() override;

    private:
      size_t m_size_in_bytes;
    };
  } // namespace renderer
}