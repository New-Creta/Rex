#pragma once

#include <wrl/client.h>

namespace rex
{
  namespace wrl
  {
    template <typename T>
    using ComPtr = Microsoft::WRL::ComPtr<T>;
  } // namespace wrl
} // namespace rex