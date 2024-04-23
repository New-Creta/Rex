#pragma once

#include "rex_engine/engine/types.h"

#include "rex_directx/directx_util.h"

#include "rex_renderer_core/iresource.h"

namespace rex
{
    namespace renderer
    {
        class Device;

        class RootSignature : public IResource
        {
        public:
            RESOURCE_CLASS_TYPE(RootSignature);

            wrl::ComPtr<ID3D12RootSignature> d3d_root_signature() const;
            D3D12_ROOT_SIGNATURE_DESC1 d3d_root_signature_description() const;

            u32 descriptor_table_bit_mask(D3D12_DESCRIPTOR_HEAP_TYPE descriptorHeapType) const;
            u32 num_descriptors(u32 rootIndex) const;

        protected:
            RootSignature(Device& device, const D3D12_ROOT_SIGNATURE_DESC1& rootSignatureDesc);

            virtual ~RootSignature();

        private:
            bool set_root_signature_desc(const D3D12_ROOT_SIGNATURE_DESC1& rootSignatureDesc);
            void destroy();

        private:
            Device& m_device;

            D3D12_ROOT_SIGNATURE_DESC1 m_root_signature_description;

            wrl::ComPtr<ID3D12RootSignature> m_root_signature;

            // Need to know the number of descriptors per descriptor table.
            // A maximum of 32 descriptor tables are supported (since a 32-bit
            // mask is used to represent the descriptor tables in the root signature.
            u32 m_num_descriptors_per_table[32];

            // A bit mask that represents the root parameter indices that are 
            // descriptor tables for Samplers.
            u32 m_sampler_table_bit_mask;
            // A bit mask that represents the root parameter indices that are 
            // CBV, UAV, and SRV descriptor tables.
            u32 m_descriptor_table_bit_mask;
        };
    }
}