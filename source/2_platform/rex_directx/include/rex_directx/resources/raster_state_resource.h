#pragma once

#include "rex_directx/resources/resource.h"
#include "rex_directx/directx_util.h"

#include "rex_engine/types.h"

namespace rex
{
    namespace renderer
    {
        class RasterStateResource : public BaseResource<D3D12_RASTERIZER_DESC>
        {
        public:
            RESOURCE_CLASS_TYPE(RasterStateResource);

            RasterStateResource(const D3D12_RASTERIZER_DESC& rs)
                :m_raster_state(rs)
            {}
            ~RasterStateResource() override = default;

            D3D12_RASTERIZER_DESC* get()
            {
                return &m_raster_state;
            }
            const D3D12_RASTERIZER_DESC* get() const
            {
                return &m_raster_state;
            }

        private:
            D3D12_RASTERIZER_DESC m_raster_state;
        };
    }
}