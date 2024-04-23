#pragma once

#include "rex_engine/engine/types.h"

#include "rex_directx/directx_util.h"
#include "rex_directx/descriptors/descriptor_allocation.h"
#include "rex_directx/resources/resource.h"

namespace rex
{
    namespace renderer
    {
        class Device;

        class Texture : public Resource
        {
        public:
            RESOURCE_CLASS_TYPE(Texture);

            /**
            * Resize the texture.
            */
            bool resize(u32 width, u32 height, u16 depthOrArraySize = 1 );

            /**
            * Get the RTV for the texture.
            */
            D3D12_CPU_DESCRIPTOR_HANDLE render_target_view() const;

            /**
            * Get the DSV for the texture.
            */
            D3D12_CPU_DESCRIPTOR_HANDLE depth_stencil_view() const;

            /**
            * Get the SRV for a resource.
            */
            D3D12_CPU_DESCRIPTOR_HANDLE shader_resource_view() const;

            /**
            * Get the UAV for the texture at a specific mip level.
            * Note: Only only supported for 1D and 2D textures.
            */
            D3D12_CPU_DESCRIPTOR_HANDLE unordered_access_view(u32 mip) const;

            bool check_SRV_support();
            bool check_RTV_support();
            bool check_UAV_support();
            bool check_DSV_support();

            /**
            * Check to see if the image format has an alpha channel.
            */
            bool has_alpha() const;

            /**
            * Check the number of bits per pixel.
            */
            size_t bits_per_pixel() const;

            static bool is_UAV_compatible_format(DXGI_FORMAT format);
            static bool is_SRGB_format(DXGI_FORMAT format);
            static bool is_BGR_format(DXGI_FORMAT format);
            static bool is_depth_format(DXGI_FORMAT format);

            // Return a typeless format from the given format.
            static DXGI_FORMAT typeless_format(DXGI_FORMAT format);
            // Return an sRGB format in the same format family.
            static DXGI_FORMAT SRGB_format(DXGI_FORMAT format);
            static DXGI_FORMAT UAV_compatable_format(DXGI_FORMAT format);

        protected:
            Texture(Device& device, const D3D12_RESOURCE_DESC& resourceDesc, const D3D12_CLEAR_VALUE* clearValue = nullptr);
            Texture(Device& device, wrl::ComPtr<ID3D12Resource> resource, const D3D12_CLEAR_VALUE* clearValue = nullptr);
            ~Texture() override;

            /**
            * Create SRV and UAVs for the resource.
            */
            void create_views();

        private:
            DescriptorAllocation m_render_target_view;
            DescriptorAllocation m_depth_stencil_view;
            DescriptorAllocation m_shader_resource_view;
            DescriptorAllocation m_unordered_access_view;
        };
    }
}
