#pragma once

#include "rex_renderer_core/format.h"
#include "rex_dxgi/dxgi_util.h"

namespace rex
{
    namespace dxgi
    {
        namespace conversions
        {
            DXGI_FORMAT to_DXGI(renderer::Format format);
        }
    }
}