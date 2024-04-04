#pragma once

#include "rex_engine/engine/types.h"
#include "rex_engine/memory/blob_view.h"
#include "rex_renderer_core/resources/input_layout.h"

namespace rex
{
  namespace rhi
  {
    struct VertexBufferDesc
    {
      memory::BlobView blob_view;
      s32 vertex_size;
      InputLayoutDesc input_layout;
    };
  }
}