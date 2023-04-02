#pragma once

#include "rex_directx/dxcomobject.h"
#include "rex_directx/util.h"

#include "rex_renderer_core/gpu.h"

#include <memory>
#include <string>

#include <wrl.h>

namespace rex
{
    namespace dxgi
    {
        class Adapter : public rex::Gpu, public rex::dxgi::ComObject<IDXGIAdapter>
        {
        public:
            Adapter(rex::dx::ComPtr<IDXGIAdapter>&& adapter, uint32 version);
            ~Adapter() final;
        };
    }
}