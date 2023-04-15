#pragma once

#include <wrl.h>

namespace rex
{
    namespace wrl
    {
        template<typename T>
        using com_ptr = Microsoft::WRL::ComPtr<T>;
    }
}