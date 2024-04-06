#pragma once

#include "rex_engine/memory/blob_view.h"

namespace rex
{
  namespace rhi
  {
    struct ConstantBufferDesc
    {
      memory::BlobView blob_view;
    };
  } // namespace rhi
} // namespace rex