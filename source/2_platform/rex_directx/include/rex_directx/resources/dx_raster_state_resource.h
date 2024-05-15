#pragma once

#include "rex_directx/utility/dx_util.h"
#include "rex_renderer_core/resource_management/resource.h"

#include "rex_engine/engine/types.h"

namespace rex
{
    namespace rhi
    {
        class RasterStateResource
        {
        public:
            explicit RasterStateResource(const D3D12_RASTERIZER_DESC& rs)
                : m_raster_state(rs)
            {}
            
            D3D12_RASTERIZER_DESC* get()
            {
              return &m_raster_state;
            }

        private:
            D3D12_RASTERIZER_DESC m_raster_state;
        };
    } // namespace renderer
} // namespace rex