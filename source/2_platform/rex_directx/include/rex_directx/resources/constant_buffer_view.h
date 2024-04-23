#pragma once

#include "rex_engine/engine/types.h"

#include "rex_directx/directx_util.h"
#include "rex_directx/descriptors/descriptor_allocation.h"
#include "rex_renderer_core/iresource.h"

#include "rex_std/memory.h"

namespace rex
{
  namespace renderer
  {
    class ConstantBuffer;
    class Device;

    class ConstantBufferView : public IResource
    {
    public:
      RESOURCE_CLASS_TYPE(ConstantBufferView);

      rsl::shared_ptr<ConstantBuffer> constant_buffer() const;

      D3D12_CPU_DESCRIPTOR_HANDLE descriptor_handle() const;

    protected:
      ConstantBufferView(Device& device, const rsl::shared_ptr<ConstantBuffer>& constantBuffer, size_t offset = 0);
      virtual ~ConstantBufferView();

    private:
      Device& m_device;
      rsl::shared_ptr<ConstantBuffer> m_constant_buffer;
      DescriptorAllocation m_descriptor_allocation;
    };
  } // namespace renderer
}