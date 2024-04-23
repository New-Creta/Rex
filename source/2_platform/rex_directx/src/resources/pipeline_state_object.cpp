#include "rex_directx/resources/pipeline_state_object.h"

#include "rex_directx/directx_device.h"

#include "rex_engine/diagnostics/assert.h"

namespace rex
{
  namespace renderer
  {
    PipelineStateObject::PipelineStateObject(Device& device, const D3D12_PIPELINE_STATE_STREAM_DESC& desc)
        : m_device(device)
    {
      auto d3d_device = device.d3d_device();

      auto hr = d3d_device->CreatePipelineState(&desc, IID_PPV_ARGS(&m_d3d_pipeline_state_object));

      REX_ASSERT_X(SUCCEEDED(hr), "Unable to CreatePipelineState");
    }

    PipelineStateObject::~PipelineStateObject() = default;

    wrl::ComPtr<ID3D12PipelineState> PipelineStateObject::d3d_pipeline_state_object() const
    {
      return m_d3d_pipeline_state_object;
    }
  } // namespace renderer
} // namespace rex