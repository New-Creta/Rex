#pragma once

#include "rex_directx/directx_util.h"
#include "rex_renderer_core/resource.h"

#include "rex_engine/engine/types.h"

namespace rex
{
    namespace renderer
    {
        class RasterStateResource : public BaseResource<D3D12_RASTERIZER_DESC>
        {
        public:
            RESOURCE_CLASS_TYPE(RasterStateResource);

            explicit RasterStateResource(const D3D12_RASTERIZER_DESC& rs)
                : BaseResource(&m_raster_state)
                ,m_raster_state(rs)
            {}
            ~RasterStateResource() override = default;

        private:
            D3D12_RASTERIZER_DESC m_raster_state;
        };
    } // namespace renderer
} // namespace rex