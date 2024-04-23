#include "rex_directx/resources/resource.h"
#include "rex_directx/resources/resource_state_tracker.h"

#include "rex_directx/directx_call.h"
#include "rex_directx/directx_device.h"

#include "rex_engine/diagnostics/assert.h"

#include "rex_directx/log.h"

namespace rex
{
  namespace renderer
  {
    //-------------------------------------------------------------------------
    Resource::Resource(Device& device, const D3D12_RESOURCE_DESC& resourceDesc, const D3D12_CLEAR_VALUE* clearValue)
        : m_device_ref(device)
    {
      auto d3d_device = m_device_ref.d3d_device();

      if(clearValue)
      {
        m_d3d_clear_value = rsl::make_unique<D3D12_CLEAR_VALUE>(*clearValue);
      }

      auto default_heap_properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

      auto hr = d3d_device->CreateCommittedResource(&default_heap_properties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_COMMON, m_d3d_clear_value.get(), IID_PPV_ARGS(&m_d3d_resource));

      REX_ASSERT_X(SUCCEEDED(hr), "Unable to CreateCommittedResource");

      ResourceStateTracker::add_global_resource_state(m_d3d_resource.Get(), D3D12_RESOURCE_STATE_COMMON);

      auto result = check_feature_support();

      REX_ASSERT_X(result, "Resource format is not supported");
    }

    //-------------------------------------------------------------------------
    Resource::Resource(Device& device, wrl::ComPtr<ID3D12Resource> Resource, const D3D12_CLEAR_VALUE* clearValue)
        : m_device_ref(device)
        , m_d3d_resource(Resource)
    {
      if(clearValue)
      {
        m_d3d_clear_value = rsl::make_unique<D3D12_CLEAR_VALUE>(*clearValue);
      }

      auto result = check_feature_support();

      REX_ASSERT_X(result, "Resource format is not supported");
    }

    //-------------------------------------------------------------------------
    Resource::~Resource() = default;

    //-------------------------------------------------------------------------
    Device* Resource::device() const
    {
      return &m_device_ref;
    }

    //-------------------------------------------------------------------------
    wrl::ComPtr<ID3D12Resource> Resource::d3d_resource() const
    {
      return m_d3d_resource;
    }

    //-------------------------------------------------------------------------
    D3D12_RESOURCE_DESC Resource::d3d_resource_desc() const
    {
      D3D12_RESOURCE_DESC res_desc = {};
      if(m_d3d_resource)
      {
        res_desc = m_d3d_resource->GetDesc();
      }

      return res_desc;
    }

    //-------------------------------------------------------------------------
    void Resource::set_resource_name(const rsl::wstring& name)
    {
      m_resource_name = name;

      if(m_d3d_resource && !m_resource_name.empty())
      {
        m_d3d_resource->SetName(m_resource_name.c_str());
      }
    }

    //-------------------------------------------------------------------------
    bool Resource::check_format_support(D3D12_FORMAT_SUPPORT1 formatSupport) const
    {
      return (m_format_support.Support1 & formatSupport) != 0;
    }

    //-------------------------------------------------------------------------
    bool Resource::check_format_support(D3D12_FORMAT_SUPPORT2 formatSupport) const
    {
      return (m_format_support.Support2 & formatSupport) != 0;
    }

    //-------------------------------------------------------------------------
    const D3D12_CLEAR_VALUE* Resource::d3d_clear_value() const
    {
      return m_d3d_clear_value.get();
    }

    //-------------------------------------------------------------------------
    const rsl::wstring& Resource::resource_name() const
    {
      return m_resource_name;
    }

    //-------------------------------------------------------------------------
    bool Resource::check_feature_support()
    {
      auto d3d_device = m_device_ref.d3d_device();

      auto desc = m_d3d_resource->GetDesc();

      m_format_support.Format = desc.Format;

      if(DX_FAILED(d3d_device->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &m_format_support, sizeof(D3D12_FEATURE_DATA_FORMAT_SUPPORT))))
      {
        REX_ERROR(LogDirectX, "Feature not supported on D3D12 Resource");
        return false;
      }

      return true;
    }
  } // namespace renderer
} // namespace rex