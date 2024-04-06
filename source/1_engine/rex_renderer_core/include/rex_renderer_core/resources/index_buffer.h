#pragma once

#include "rex_engine/engine/types.h"
#include "rex_engine/memory/blob_view.h"
#include "rex_renderer_core/rendering/index_buffer_format.h"

namespace rex
{
  namespace rhi
  {
    struct IndexBufferDesc
    {
      memory::BlobView blob_view;
      renderer::IndexBufferFormat format;
      s32 index_count;
      s32 base_vertex_loc;
    };
  } // namespace rhi
} // namespace rex