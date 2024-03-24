#pragma once

#include "rex_engine/memory/blob_view.h"

namespace rex
{
  namespace rhi
  {
    struct BufferDesc
    {
      memory::BlobView blob_view;
    };
  }
}