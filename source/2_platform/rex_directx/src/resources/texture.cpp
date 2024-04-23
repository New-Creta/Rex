#include "rex_directx/resources/texture.h"
#include "rex_directx/resources/resource_state_tracker.h"

#include "rex_directx/directx_call.h"
#include "rex_directx/directx_device.h"

#include "rex_directx/log.h"

#include "rex_engine/diagnostics/assert.h"

#include <DirectXTex/DirectXTex.h>

// Hashers for view descriptions.
namespace std
{
  // Source: https://stackoverflow.com/questions/2590677/how-do-i-combine-hash-values-in-c0x
  template <typename T>
  inline void hash_combine(rsl::size_t& seed, const T& v)
  {
    rsl::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  }

  template <>
  struct hash<D3D12_SHADER_RESOURCE_VIEW_DESC>
  {
    rsl::size_t operator()(const D3D12_SHADER_RESOURCE_VIEW_DESC& srvDesc) const noexcept
    {
      rsl::size_t seed = 0;

      hash_combine(seed, srvDesc.Format);
      hash_combine(seed, srvDesc.ViewDimension);
      hash_combine(seed, srvDesc.Shader4ComponentMapping);

      switch(srvDesc.ViewDimension)
      {
        case D3D12_SRV_DIMENSION_BUFFER:
          hash_combine(seed, srvDesc.Buffer.FirstElement);
          hash_combine(seed, srvDesc.Buffer.NumElements);
          hash_combine(seed, srvDesc.Buffer.StructureByteStride);
          hash_combine(seed, srvDesc.Buffer.Flags);
          break;
        case D3D12_SRV_DIMENSION_TEXTURE1D:
          hash_combine(seed, srvDesc.Texture1D.MostDetailedMip);
          hash_combine(seed, srvDesc.Texture1D.MipLevels);
          hash_combine(seed, srvDesc.Texture1D.ResourceMinLODClamp);
          break;
        case D3D12_SRV_DIMENSION_TEXTURE1DARRAY:
          hash_combine(seed, srvDesc.Texture1DArray.MostDetailedMip);
          hash_combine(seed, srvDesc.Texture1DArray.MipLevels);
          hash_combine(seed, srvDesc.Texture1DArray.FirstArraySlice);
          hash_combine(seed, srvDesc.Texture1DArray.ArraySize);
          hash_combine(seed, srvDesc.Texture1DArray.ResourceMinLODClamp);
          break;
        case D3D12_SRV_DIMENSION_TEXTURE2D:
          hash_combine(seed, srvDesc.Texture2D.MostDetailedMip);
          hash_combine(seed, srvDesc.Texture2D.MipLevels);
          hash_combine(seed, srvDesc.Texture2D.PlaneSlice);
          hash_combine(seed, srvDesc.Texture2D.ResourceMinLODClamp);
          break;
        case D3D12_SRV_DIMENSION_TEXTURE2DARRAY:
          hash_combine(seed, srvDesc.Texture2DArray.MostDetailedMip);
          hash_combine(seed, srvDesc.Texture2DArray.MipLevels);
          hash_combine(seed, srvDesc.Texture2DArray.FirstArraySlice);
          hash_combine(seed, srvDesc.Texture2DArray.ArraySize);
          hash_combine(seed, srvDesc.Texture2DArray.PlaneSlice);
          hash_combine(seed, srvDesc.Texture2DArray.ResourceMinLODClamp);
          break;
        case D3D12_SRV_DIMENSION_TEXTURE2DMS:
          //                hash_combine(seed, srvDesc.Texture2DMS.UnusedField_NothingToDefine);
          break;
        case D3D12_SRV_DIMENSION_TEXTURE2DMSARRAY:
          hash_combine(seed, srvDesc.Texture2DMSArray.FirstArraySlice);
          hash_combine(seed, srvDesc.Texture2DMSArray.ArraySize);
          break;
        case D3D12_SRV_DIMENSION_TEXTURE3D:
          hash_combine(seed, srvDesc.Texture3D.MostDetailedMip);
          hash_combine(seed, srvDesc.Texture3D.MipLevels);
          hash_combine(seed, srvDesc.Texture3D.ResourceMinLODClamp);
          break;
        case D3D12_SRV_DIMENSION_TEXTURECUBE:
          hash_combine(seed, srvDesc.TextureCube.MostDetailedMip);
          hash_combine(seed, srvDesc.TextureCube.MipLevels);
          hash_combine(seed, srvDesc.TextureCube.ResourceMinLODClamp);
          break;
        case D3D12_SRV_DIMENSION_TEXTURECUBEARRAY:
          hash_combine(seed, srvDesc.TextureCubeArray.MostDetailedMip);
          hash_combine(seed, srvDesc.TextureCubeArray.MipLevels);
          hash_combine(seed, srvDesc.TextureCubeArray.First2DArrayFace);
          hash_combine(seed, srvDesc.TextureCubeArray.NumCubes);
          hash_combine(seed, srvDesc.TextureCubeArray.ResourceMinLODClamp);
          break;
          // TODO: Update Visual Studio?
          // case D3D12_SRV_DIMENSION_RAYTRACING_ACCELERATION_STRUCTURE:
          //    hash_combine(seed, srvDesc.RaytracingAccelerationStructure.Location);
          //    break;
      }

      return seed;
    }
  };

  template <>
  struct hash<D3D12_UNORDERED_ACCESS_VIEW_DESC>
  {
    rsl::size_t operator()(const D3D12_UNORDERED_ACCESS_VIEW_DESC& uavDesc) const noexcept
    {
      rsl::size_t seed = 0;

      hash_combine(seed, uavDesc.Format);
      hash_combine(seed, uavDesc.ViewDimension);

      switch(uavDesc.ViewDimension)
      {
        case D3D12_UAV_DIMENSION_BUFFER:
          hash_combine(seed, uavDesc.Buffer.FirstElement);
          hash_combine(seed, uavDesc.Buffer.NumElements);
          hash_combine(seed, uavDesc.Buffer.StructureByteStride);
          hash_combine(seed, uavDesc.Buffer.CounterOffsetInBytes);
          hash_combine(seed, uavDesc.Buffer.Flags);
          break;
        case D3D12_UAV_DIMENSION_TEXTURE1D: hash_combine(seed, uavDesc.Texture1D.MipSlice); break;
        case D3D12_UAV_DIMENSION_TEXTURE1DARRAY:
          hash_combine(seed, uavDesc.Texture1DArray.MipSlice);
          hash_combine(seed, uavDesc.Texture1DArray.FirstArraySlice);
          hash_combine(seed, uavDesc.Texture1DArray.ArraySize);
          break;
        case D3D12_UAV_DIMENSION_TEXTURE2D:
          hash_combine(seed, uavDesc.Texture2D.MipSlice);
          hash_combine(seed, uavDesc.Texture2D.PlaneSlice);
          break;
        case D3D12_UAV_DIMENSION_TEXTURE2DARRAY:
          hash_combine(seed, uavDesc.Texture2DArray.MipSlice);
          hash_combine(seed, uavDesc.Texture2DArray.FirstArraySlice);
          hash_combine(seed, uavDesc.Texture2DArray.ArraySize);
          hash_combine(seed, uavDesc.Texture2DArray.PlaneSlice);
          break;
        case D3D12_UAV_DIMENSION_TEXTURE3D:
          hash_combine(seed, uavDesc.Texture3D.MipSlice);
          hash_combine(seed, uavDesc.Texture3D.FirstWSlice);
          hash_combine(seed, uavDesc.Texture3D.WSize);
          break;
      }

      return seed;
    }
  };
} // namespace std

namespace rex
{
  namespace renderer
  {
    namespace internal
    {
      // Get a UAV description that matches the resource description.
      D3D12_UNORDERED_ACCESS_VIEW_DESC uav_desc(const D3D12_RESOURCE_DESC& res_desc, UINT mipSlice, UINT arraySlice = 0, UINT planeSlice = 0)
      {
        D3D12_UNORDERED_ACCESS_VIEW_DESC uav_desc = {};
        uav_desc.Format                           = res_desc.Format;

        switch(res_desc.Dimension)
        {
          case D3D12_RESOURCE_DIMENSION_TEXTURE1D:
            if(res_desc.DepthOrArraySize > 1)
            {
              uav_desc.ViewDimension                  = D3D12_UAV_DIMENSION_TEXTURE1DARRAY;
              uav_desc.Texture1DArray.ArraySize       = res_desc.DepthOrArraySize - arraySlice;
              uav_desc.Texture1DArray.FirstArraySlice = arraySlice;
              uav_desc.Texture1DArray.MipSlice        = mipSlice;
            }
            else
            {
              uav_desc.ViewDimension      = D3D12_UAV_DIMENSION_TEXTURE1D;
              uav_desc.Texture1D.MipSlice = mipSlice;
            }
            break;
          case D3D12_RESOURCE_DIMENSION_TEXTURE2D:
            if(res_desc.DepthOrArraySize > 1)
            {
              uav_desc.ViewDimension                  = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
              uav_desc.Texture2DArray.ArraySize       = res_desc.DepthOrArraySize - arraySlice;
              uav_desc.Texture2DArray.FirstArraySlice = arraySlice;
              uav_desc.Texture2DArray.PlaneSlice      = planeSlice;
              uav_desc.Texture2DArray.MipSlice        = mipSlice;
            }
            else
            {
              uav_desc.ViewDimension        = D3D12_UAV_DIMENSION_TEXTURE2D;
              uav_desc.Texture2D.PlaneSlice = planeSlice;
              uav_desc.Texture2D.MipSlice   = mipSlice;
            }
            break;
          case D3D12_RESOURCE_DIMENSION_TEXTURE3D:
            uav_desc.ViewDimension         = D3D12_UAV_DIMENSION_TEXTURE3D;
            uav_desc.Texture3D.WSize       = res_desc.DepthOrArraySize - arraySlice;
            uav_desc.Texture3D.FirstWSlice = arraySlice;
            uav_desc.Texture3D.MipSlice    = mipSlice;
            break;
          default: REX_ASSERT("Invalid resource dimension.");
        }

        return uav_desc;
      }
    } // namespace internal

    Texture::Texture(Device& device, const D3D12_RESOURCE_DESC& resourceDesc, const D3D12_CLEAR_VALUE* clearValue)
        : Resource(device, resourceDesc, clearValue)
    {
      create_views();
    }
    Texture::Texture(Device& device, wrl::ComPtr<ID3D12Resource> resource, const D3D12_CLEAR_VALUE* clearValue)
        : Resource(device, resource, clearValue)
    {
      create_views();
    }
    Texture::~Texture() = default;

    bool Texture::resize(u32 width, u32 height, u16 depthOrArraySize)
    {
      // Resource can't be resized if it was never created in the first place.
      if(d3d_resource())
      {
        CD3DX12_RESOURCE_DESC res_desc(d3d_resource()->GetDesc());

        res_desc.Width            = (rsl::max)(width, 1u);
        res_desc.Height           = (rsl::max)(height, 1u);
        res_desc.DepthOrArraySize = depthOrArraySize;
        res_desc.MipLevels        = res_desc.SampleDesc.Count > 1 ? 1 : 0;

        auto d3d_device = device()->d3d_device();
        auto default_heap_properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

        if(DX_FAILED(d3d_device->CreateCommittedResource(&default_heap_properties, D3D12_HEAP_FLAG_NONE, &res_desc, D3D12_RESOURCE_STATE_COMMON, d3d_clear_value(), IID_PPV_ARGS(&d3d_resource()))))
        {
          REX_ERROR(LogDirectX, "Failed to CreateCommittedResource");
          return false;
        }

        // Retain the name of the resource if one was already specified.
        d3d_resource()->SetName(resource_name().c_str());

        ResourceStateTracker::add_global_resource_state(d3d_resource().Get(), D3D12_RESOURCE_STATE_COMMON);

        create_views();

        return true;
      }

      return false;
    }

    D3D12_CPU_DESCRIPTOR_HANDLE Texture::render_target_view() const
    {
      return m_render_target_view.descriptor_handle();
    }

    D3D12_CPU_DESCRIPTOR_HANDLE Texture::depth_stencil_view() const
    {
      return m_depth_stencil_view.descriptor_handle();
    }

    D3D12_CPU_DESCRIPTOR_HANDLE Texture::shader_resource_view() const
    {
      return m_shader_resource_view.descriptor_handle();
    }

    D3D12_CPU_DESCRIPTOR_HANDLE Texture::unordered_access_view(u32 mip) const
    {
      return m_unordered_access_view.descriptor_handle(mip);
    }

    bool Texture::check_SRV_support()
    {
      return check_format_support(D3D12_FORMAT_SUPPORT1_SHADER_SAMPLE);
    }

    bool Texture::check_RTV_support()
    {
      return check_format_support(D3D12_FORMAT_SUPPORT1_RENDER_TARGET);
    }

    bool Texture::check_UAV_support()
    {
      return check_format_support(D3D12_FORMAT_SUPPORT1_TYPED_UNORDERED_ACCESS_VIEW) && check_format_support(D3D12_FORMAT_SUPPORT2_UAV_TYPED_LOAD) && check_format_support(D3D12_FORMAT_SUPPORT2_UAV_TYPED_STORE);
    }

    bool Texture::check_DSV_support()
    {
      return check_format_support(D3D12_FORMAT_SUPPORT1_DEPTH_STENCIL);
    }

    bool Texture::has_alpha() const
    {
      DXGI_FORMAT format = d3d_resource_desc().Format;

      bool has_alpha = false;

      switch(format)
      {
        case DXGI_FORMAT_R32G32B32A32_TYPELESS:
        case DXGI_FORMAT_R32G32B32A32_FLOAT:
        case DXGI_FORMAT_R32G32B32A32_UINT:
        case DXGI_FORMAT_R32G32B32A32_SINT:
        case DXGI_FORMAT_R16G16B16A16_TYPELESS:
        case DXGI_FORMAT_R16G16B16A16_FLOAT:
        case DXGI_FORMAT_R16G16B16A16_UNORM:
        case DXGI_FORMAT_R16G16B16A16_UINT:
        case DXGI_FORMAT_R16G16B16A16_SNORM:
        case DXGI_FORMAT_R16G16B16A16_SINT:
        case DXGI_FORMAT_R10G10B10A2_TYPELESS:
        case DXGI_FORMAT_R10G10B10A2_UNORM:
        case DXGI_FORMAT_R10G10B10A2_UINT:
        case DXGI_FORMAT_R8G8B8A8_TYPELESS:
        case DXGI_FORMAT_R8G8B8A8_UNORM:
        case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
        case DXGI_FORMAT_R8G8B8A8_UINT:
        case DXGI_FORMAT_R8G8B8A8_SNORM:
        case DXGI_FORMAT_R8G8B8A8_SINT:
        case DXGI_FORMAT_BC1_TYPELESS:
        case DXGI_FORMAT_BC1_UNORM:
        case DXGI_FORMAT_BC1_UNORM_SRGB:
        case DXGI_FORMAT_BC2_TYPELESS:
        case DXGI_FORMAT_BC2_UNORM:
        case DXGI_FORMAT_BC2_UNORM_SRGB:
        case DXGI_FORMAT_BC3_TYPELESS:
        case DXGI_FORMAT_BC3_UNORM:
        case DXGI_FORMAT_BC3_UNORM_SRGB:
        case DXGI_FORMAT_B5G5R5A1_UNORM:
        case DXGI_FORMAT_B8G8R8A8_UNORM:
        case DXGI_FORMAT_B8G8R8X8_UNORM:
        case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
        case DXGI_FORMAT_B8G8R8A8_TYPELESS:
        case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
        case DXGI_FORMAT_B8G8R8X8_TYPELESS:
        case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
        case DXGI_FORMAT_BC6H_TYPELESS:
        case DXGI_FORMAT_BC7_TYPELESS:
        case DXGI_FORMAT_BC7_UNORM:
        case DXGI_FORMAT_BC7_UNORM_SRGB:
        case DXGI_FORMAT_A8P8:
        case DXGI_FORMAT_B4G4R4A4_UNORM: has_alpha = true; break;
      }

      return has_alpha;
    }

    size_t Texture::bits_per_pixel() const
    {
      auto format = d3d_resource_desc().Format;
      return DirectX::BitsPerPixel(format);
    }

    bool Texture::is_UAV_compatible_format(DXGI_FORMAT format)
    {
      switch(format)
      {
        case DXGI_FORMAT_R32G32B32A32_FLOAT:
        case DXGI_FORMAT_R32G32B32A32_UINT:
        case DXGI_FORMAT_R32G32B32A32_SINT:
        case DXGI_FORMAT_R16G16B16A16_FLOAT:
        case DXGI_FORMAT_R16G16B16A16_UINT:
        case DXGI_FORMAT_R16G16B16A16_SINT:
        case DXGI_FORMAT_R8G8B8A8_UNORM:
        case DXGI_FORMAT_R8G8B8A8_UINT:
        case DXGI_FORMAT_R8G8B8A8_SINT:
        case DXGI_FORMAT_R32_FLOAT:
        case DXGI_FORMAT_R32_UINT:
        case DXGI_FORMAT_R32_SINT:
        case DXGI_FORMAT_R16_FLOAT:
        case DXGI_FORMAT_R16_UINT:
        case DXGI_FORMAT_R16_SINT:
        case DXGI_FORMAT_R8_UNORM:
        case DXGI_FORMAT_R8_UINT:
        case DXGI_FORMAT_R8_SINT: return true;
        default: return false;
      }
    }

    bool Texture::is_SRGB_format(DXGI_FORMAT format)
    {
      switch(format)
      {
        case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
        case DXGI_FORMAT_BC1_UNORM_SRGB:
        case DXGI_FORMAT_BC2_UNORM_SRGB:
        case DXGI_FORMAT_BC3_UNORM_SRGB:
        case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
        case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
        case DXGI_FORMAT_BC7_UNORM_SRGB: return true;
        default: return false;
      }
    }

    bool Texture::is_BGR_format(DXGI_FORMAT format)
    {
      switch(format)
      {
        case DXGI_FORMAT_B8G8R8A8_UNORM:
        case DXGI_FORMAT_B8G8R8X8_UNORM:
        case DXGI_FORMAT_B8G8R8A8_TYPELESS:
        case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
        case DXGI_FORMAT_B8G8R8X8_TYPELESS:
        case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB: return true;
        default: return false;
      }
    }

    bool Texture::is_depth_format(DXGI_FORMAT format)
    {
      switch(format)
      {
        case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
        case DXGI_FORMAT_D32_FLOAT:
        case DXGI_FORMAT_D24_UNORM_S8_UINT:
        case DXGI_FORMAT_D16_UNORM: return true;
        default: return false;
      }
    }

    DXGI_FORMAT Texture::typeless_format(DXGI_FORMAT format)
    {
      DXGI_FORMAT typeless_format = format;

      switch(format)
      {
        case DXGI_FORMAT_R32G32B32A32_FLOAT:
        case DXGI_FORMAT_R32G32B32A32_UINT:
        case DXGI_FORMAT_R32G32B32A32_SINT: typeless_format = DXGI_FORMAT_R32G32B32A32_TYPELESS; break;
        case DXGI_FORMAT_R32G32B32_FLOAT:
        case DXGI_FORMAT_R32G32B32_UINT:
        case DXGI_FORMAT_R32G32B32_SINT: typeless_format = DXGI_FORMAT_R32G32B32_TYPELESS; break;
        case DXGI_FORMAT_R16G16B16A16_FLOAT:
        case DXGI_FORMAT_R16G16B16A16_UNORM:
        case DXGI_FORMAT_R16G16B16A16_UINT:
        case DXGI_FORMAT_R16G16B16A16_SNORM:
        case DXGI_FORMAT_R16G16B16A16_SINT: typeless_format = DXGI_FORMAT_R16G16B16A16_TYPELESS; break;
        case DXGI_FORMAT_R32G32_FLOAT:
        case DXGI_FORMAT_R32G32_UINT:
        case DXGI_FORMAT_R32G32_SINT: typeless_format = DXGI_FORMAT_R32G32_TYPELESS; break;
        case DXGI_FORMAT_D32_FLOAT_S8X24_UINT: typeless_format = DXGI_FORMAT_R32G8X24_TYPELESS; break;
        case DXGI_FORMAT_R10G10B10A2_UNORM:
        case DXGI_FORMAT_R10G10B10A2_UINT: typeless_format = DXGI_FORMAT_R10G10B10A2_TYPELESS; break;
        case DXGI_FORMAT_R8G8B8A8_UNORM:
        case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
        case DXGI_FORMAT_R8G8B8A8_UINT:
        case DXGI_FORMAT_R8G8B8A8_SNORM:
        case DXGI_FORMAT_R8G8B8A8_SINT: typeless_format = DXGI_FORMAT_R8G8B8A8_TYPELESS; break;
        case DXGI_FORMAT_R16G16_FLOAT:
        case DXGI_FORMAT_R16G16_UNORM:
        case DXGI_FORMAT_R16G16_UINT:
        case DXGI_FORMAT_R16G16_SNORM:
        case DXGI_FORMAT_R16G16_SINT: typeless_format = DXGI_FORMAT_R16G16_TYPELESS; break;
        case DXGI_FORMAT_D32_FLOAT:
        case DXGI_FORMAT_R32_FLOAT:
        case DXGI_FORMAT_R32_UINT:
        case DXGI_FORMAT_R32_SINT: typeless_format = DXGI_FORMAT_R32_TYPELESS; break;
        case DXGI_FORMAT_R8G8_UNORM:
        case DXGI_FORMAT_R8G8_UINT:
        case DXGI_FORMAT_R8G8_SNORM:
        case DXGI_FORMAT_R8G8_SINT: typeless_format = DXGI_FORMAT_R8G8_TYPELESS; break;
        case DXGI_FORMAT_R16_FLOAT:
        case DXGI_FORMAT_D16_UNORM:
        case DXGI_FORMAT_R16_UNORM:
        case DXGI_FORMAT_R16_UINT:
        case DXGI_FORMAT_R16_SNORM:
        case DXGI_FORMAT_R16_SINT: typeless_format = DXGI_FORMAT_R16_TYPELESS;
        case DXGI_FORMAT_R8_UNORM:
        case DXGI_FORMAT_R8_UINT:
        case DXGI_FORMAT_R8_SNORM:
        case DXGI_FORMAT_R8_SINT: typeless_format = DXGI_FORMAT_R8_TYPELESS; break;
        case DXGI_FORMAT_BC1_UNORM:
        case DXGI_FORMAT_BC1_UNORM_SRGB: typeless_format = DXGI_FORMAT_BC1_TYPELESS; break;
        case DXGI_FORMAT_BC2_UNORM:
        case DXGI_FORMAT_BC2_UNORM_SRGB: typeless_format = DXGI_FORMAT_BC2_TYPELESS; break;
        case DXGI_FORMAT_BC3_UNORM:
        case DXGI_FORMAT_BC3_UNORM_SRGB: typeless_format = DXGI_FORMAT_BC3_TYPELESS; break;
        case DXGI_FORMAT_BC4_UNORM:
        case DXGI_FORMAT_BC4_SNORM: typeless_format = DXGI_FORMAT_BC4_TYPELESS; break;
        case DXGI_FORMAT_BC5_UNORM:
        case DXGI_FORMAT_BC5_SNORM: typeless_format = DXGI_FORMAT_BC5_TYPELESS; break;
        case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB: typeless_format = DXGI_FORMAT_B8G8R8A8_TYPELESS; break;
        case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB: typeless_format = DXGI_FORMAT_B8G8R8X8_TYPELESS; break;
        case DXGI_FORMAT_BC6H_UF16:
        case DXGI_FORMAT_BC6H_SF16: typeless_format = DXGI_FORMAT_BC6H_TYPELESS; break;
        case DXGI_FORMAT_BC7_UNORM:
        case DXGI_FORMAT_BC7_UNORM_SRGB: typeless_format = DXGI_FORMAT_BC7_TYPELESS; break;
      }

      return typeless_format;
    }

    DXGI_FORMAT Texture::SRGB_format(DXGI_FORMAT format)
    {
      DXGI_FORMAT srgb_format = format;
      switch(format)
      {
        case DXGI_FORMAT_R8G8B8A8_UNORM: srgb_format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; break;
        case DXGI_FORMAT_BC1_UNORM: srgb_format = DXGI_FORMAT_BC1_UNORM_SRGB; break;
        case DXGI_FORMAT_BC2_UNORM: srgb_format = DXGI_FORMAT_BC2_UNORM_SRGB; break;
        case DXGI_FORMAT_BC3_UNORM: srgb_format = DXGI_FORMAT_BC3_UNORM_SRGB; break;
        case DXGI_FORMAT_B8G8R8A8_UNORM: srgb_format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB; break;
        case DXGI_FORMAT_B8G8R8X8_UNORM: srgb_format = DXGI_FORMAT_B8G8R8X8_UNORM_SRGB; break;
        case DXGI_FORMAT_BC7_UNORM: srgb_format = DXGI_FORMAT_BC7_UNORM_SRGB; break;
      }

      return srgb_format;
    }

    DXGI_FORMAT Texture::UAV_compatable_format(DXGI_FORMAT format)
    {
      DXGI_FORMAT uav_format = format;

      switch(format)
      {
        case DXGI_FORMAT_R8G8B8A8_TYPELESS:
        case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
        case DXGI_FORMAT_B8G8R8A8_UNORM:
        case DXGI_FORMAT_B8G8R8X8_UNORM:
        case DXGI_FORMAT_B8G8R8A8_TYPELESS:
        case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
        case DXGI_FORMAT_B8G8R8X8_TYPELESS:
        case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB: uav_format = DXGI_FORMAT_R8G8B8A8_UNORM; break;
        case DXGI_FORMAT_R32_TYPELESS:
        case DXGI_FORMAT_D32_FLOAT: uav_format = DXGI_FORMAT_R32_FLOAT; break;
      }

      return uav_format;
    }

    void Texture::create_views()
    {
      if(d3d_resource())
      {
        auto d          = device();
        auto d3d_device = d->d3d_device();

        CD3DX12_RESOURCE_DESC desc(d3d_resource()->GetDesc());

        // Create RTV
        if((desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET) != 0 && check_RTV_support())
        {
          m_render_target_view = d->allocate_descriptors(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
          d3d_device->CreateRenderTargetView(d3d_resource().Get(), nullptr, m_render_target_view.descriptor_handle());
        }
        // Create DSV
        if((desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL) != 0 && check_DSV_support())
        {
          m_depth_stencil_view = d->allocate_descriptors(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
          d3d_device->CreateDepthStencilView(d3d_resource().Get(), nullptr, m_depth_stencil_view.descriptor_handle());
        }
        // Create SRV
        if((desc.Flags & D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE) == 0 && check_SRV_support())
        {
          m_shader_resource_view = d->allocate_descriptors(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
          d3d_device->CreateShaderResourceView(d3d_resource().Get(), nullptr, m_shader_resource_view.descriptor_handle());
        }
        // Create UAV for each mip (only supported for 1D and 2D textures).
        if((desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS) != 0 && check_UAV_support() && desc.DepthOrArraySize == 1)
        {
          m_unordered_access_view = d->allocate_descriptors(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, desc.MipLevels);
          for(int i = 0; i < desc.MipLevels; ++i)
          {
            auto uavDesc = internal::uav_desc(desc, i);
            d3d_device->CreateUnorderedAccessView(d3d_resource().Get(), nullptr, &uavDesc, m_unordered_access_view.descriptor_handle(i));
          }
        }
      }
    }
  } // namespace renderer
} // namespace rex