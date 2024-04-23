#pragma once

#include "rex_engine/engine/types.h"

#include "rex_directx/directx_util.h"

#include "rex_renderer_core/iresource.h"

namespace rex
{
  namespace renderer
  {
    class Device;

    class PipelineStateObject : public IResource
    {
    public:
      RESOURCE_CLASS_TYPE(PipelineStateObject);

      wrl::ComPtr<ID3D12PipelineState> d3d_pipeline_state_object() const;

    protected:
      PipelineStateObject(Device& device, const D3D12_PIPELINE_STATE_STREAM_DESC& desc);
      virtual ~PipelineStateObject();

    private:
      Device& m_device;
      wrl::ComPtr<ID3D12PipelineState> m_d3d_pipeline_state_object;
    };
  } // namespace renderer
}