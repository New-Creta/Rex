#pragma once

#include "rex_directx/utility/directx_util.h"
#include "rex_renderer_core/resource_management/resource.h"

#include "rex_engine/engine/types.h"

namespace rex
{
    namespace rhi
    {
        class RasterStateResource : public BaseResource<D3D12_RASTERIZER_DESC>
        {
        public:
            explicit RasterStateResource(ResourceHash hash, const D3D12_RASTERIZER_DESC& rs)
                : BaseResource(&m_raster_state, hash)
                ,m_raster_state(rs)
            {}
            ~RasterStateResource() override = default;

        private:
            D3D12_RASTERIZER_DESC m_raster_state;
        };
    } // namespace renderer
} // namespace rex