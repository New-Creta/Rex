#include "rex_directx/resources/root_signature.h"

#include "rex_directx/directx_call.h"
#include "rex_directx/directx_device.h"

#include "rex_engine/diagnostics/assert.h"

#include "rex_directx/log.h"

namespace rex
{
  namespace renderer
  {
    RootSignature::RootSignature(Device& device, const D3D12_ROOT_SIGNATURE_DESC1& rootSignatureDesc)
        : m_device(device)
        , m_root_signature_description {}
        , m_num_descriptors_per_table {0}
        , m_sampler_table_bit_mask(0)
        , m_descriptor_table_bit_mask(0)
    {
      set_root_signature_desc(rootSignatureDesc);
    }

    RootSignature::~RootSignature()
    {
      destroy();
    }

    bool RootSignature::set_root_signature_desc(const D3D12_ROOT_SIGNATURE_DESC1& rootSignatureDesc)
    {
      // Make sure any previously allocated root signature description is cleaned
      // up first.
      destroy();

      u32 num_parameters                = rootSignatureDesc.NumParameters;
      D3D12_ROOT_PARAMETER1* parameters = num_parameters > 0 ? new D3D12_ROOT_PARAMETER1[num_parameters] : nullptr;

      for(u32 i = 0; i < num_parameters; ++i)
      {
        const D3D12_ROOT_PARAMETER1& root_parameter = rootSignatureDesc.pParameters[i];
        parameters[i]                               = root_parameter;

        if(root_parameter.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE)
        {
          u32 num_descriptor_ranges                  = root_parameter.DescriptorTable.NumDescriptorRanges;
          D3D12_DESCRIPTOR_RANGE1* descriptor_ranges = num_descriptor_ranges > 0 ? new D3D12_DESCRIPTOR_RANGE1[num_descriptor_ranges] : nullptr;

          memcpy(descriptor_ranges, root_parameter.DescriptorTable.pDescriptorRanges, sizeof(D3D12_DESCRIPTOR_RANGE1) * num_descriptor_ranges);

          parameters[i].DescriptorTable.NumDescriptorRanges = num_descriptor_ranges;
          parameters[i].DescriptorTable.pDescriptorRanges   = descriptor_ranges;

          // Set the bit mask depending on the type of descriptor table.
          if(num_descriptor_ranges > 0)
          {
            switch(descriptor_ranges[0].RangeType)
            {
              case D3D12_DESCRIPTOR_RANGE_TYPE_CBV:
              case D3D12_DESCRIPTOR_RANGE_TYPE_SRV:
              case D3D12_DESCRIPTOR_RANGE_TYPE_UAV: m_descriptor_table_bit_mask |= (1 << i); break;
              case D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER: m_sampler_table_bit_mask |= (1 << i); break;
            }
          }

          // Count the number of descriptors in the descriptor table.
          for(u32 j = 0; j < num_descriptor_ranges; ++j)
          {
            m_num_descriptors_per_table[i] += descriptor_ranges[j].NumDescriptors;
          }
        }
      }

      m_root_signature_description.NumParameters = num_parameters;
      m_root_signature_description.pParameters   = parameters;

      u32 num_static_samplers                    = rootSignatureDesc.NumStaticSamplers;
      D3D12_STATIC_SAMPLER_DESC* static_samplers = num_static_samplers > 0 ? new D3D12_STATIC_SAMPLER_DESC[num_static_samplers] : nullptr;

      if(static_samplers)
      {
        memcpy(static_samplers, rootSignatureDesc.pStaticSamplers, sizeof(D3D12_STATIC_SAMPLER_DESC) * num_static_samplers);
      }

      m_root_signature_description.NumStaticSamplers = num_static_samplers;
      m_root_signature_description.pStaticSamplers   = static_samplers;

      D3D12_ROOT_SIGNATURE_FLAGS flags   = rootSignatureDesc.Flags;
      m_root_signature_description.Flags = flags;

      CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC version_root_signature_desc;
      version_root_signature_desc.Init_1_1(num_parameters, parameters, num_static_samplers, static_samplers, flags);

      D3D_ROOT_SIGNATURE_VERSION highest_version = m_device.highest_root_signature_version();

      // Serialize the root signature.
      wrl::ComPtr<ID3DBlob> root_signature_blob;
      wrl::ComPtr<ID3DBlob> error_blob;

      if(DX_FAILED(D3DX12SerializeVersionedRootSignature(&version_root_signature_desc, highest_version, &root_signature_blob, &error_blob)))
      {
        REX_ERROR(LogDirectX, "Failed to SerializeVersionedRootSignature");
        return false;
      }

      auto d3d_device = m_device.d3d_device();

      // Create the root signature.
      if(DX_FAILED(d3d_device->CreateRootSignature(0, root_signature_blob->GetBufferPointer(), root_signature_blob->GetBufferSize(), IID_PPV_ARGS(&m_root_signature))))
      {
        REX_ERROR(LogDirectX, "Failed to CreateRootSignature");
        return false;
      }

      return true;
    }

    wrl::ComPtr<ID3D12RootSignature> RootSignature::d3d_root_signature() const
    {
      return m_root_signature;
    }

    D3D12_ROOT_SIGNATURE_DESC1 RootSignature::d3d_root_signature_description() const
    {
      return m_root_signature_description;
    }

    u32 RootSignature::descriptor_table_bit_mask(D3D12_DESCRIPTOR_HEAP_TYPE descriptorHeapType) const
    {
      u32 descriptor_table_bit_mask = 0;
      switch(descriptorHeapType)
      {
        case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV: descriptor_table_bit_mask = m_descriptor_table_bit_mask; break;
        case D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER: descriptor_table_bit_mask = m_sampler_table_bit_mask; break;
      }

      return descriptor_table_bit_mask;
    }

    u32 RootSignature::num_descriptors(u32 rootIndex) const
    {
      REX_ASSERT_X(rootIndex < 32, "Root index exceeded max amount of descriptors ( 32 )");
      return m_num_descriptors_per_table[rootIndex];
    }

    void RootSignature::destroy()
    {
      for(u32 i = 0; i < m_root_signature_description.NumParameters; ++i)
      {
        const D3D12_ROOT_PARAMETER1& root_parameter = m_root_signature_description.pParameters[i];

        if(root_parameter.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE)
        {
          delete[] root_parameter.DescriptorTable.pDescriptorRanges;
        }
      }

      delete[] m_root_signature_description.pParameters;
      m_root_signature_description.pParameters   = nullptr;
      m_root_signature_description.NumParameters = 0;

      delete[] m_root_signature_description.pStaticSamplers;
      m_root_signature_description.pStaticSamplers   = nullptr;
      m_root_signature_description.NumStaticSamplers = 0;

      m_descriptor_table_bit_mask = 0;
      m_sampler_table_bit_mask    = 0;

      memset(m_num_descriptors_per_table, 0, sizeof(m_num_descriptors_per_table));
    }
  } // namespace renderer
} // namespace rex