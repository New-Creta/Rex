#pragma once

#include <wrl/client.h> // for ComPtr

namespace rex
{
  namespace wrl
  {
    template <typename T>
    using com_ptr = Microsoft::WRL::ComPtr<T>;
  } // namespace wrl
} // namespace rex