#pragma once

#include "rex_engine/engine/types.h"
#include "rex_renderer_core/rhi/input_layout_classification.h"
#include "rex_renderer_core/rhi/vertex_buffer_format.h"
#include "rex_std/string_view.h"

namespace rex
{
  namespace rhi
  {
    struct InputLayoutElementDesc
    {
      rsl::string_view semantic_name;
      renderer::VertexBufferFormat format;
      renderer::InputLayoutClassification input_slot_class;
      s32 semantic_index;
      s32 input_slot;
      s32 aligned_byte_offset;
      s32 instance_data_step_rate;
    };

    struct InputLayoutDesc
    {
      rsl::vector<InputLayoutElementDesc> input_layout {};
    };

    class InputLayout
    {

    };
  } // namespace rhi
} // namespace rex