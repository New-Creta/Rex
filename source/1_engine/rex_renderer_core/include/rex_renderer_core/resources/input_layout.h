#pragma once

#include "rex_engine/engine/types.h"
#include "rex_renderer_core/gfx/input_layout_classification.h"
#include "rex_renderer_core/gfx/vertex_buffer_format.h"
#include "rex_std/string_view.h"

namespace rex
{
  namespace gfx
  {
    struct InputLayoutElementDesc
    {
      rsl::string_view semantic_name;                       // The HLSL semantic name associated with the element (will change when we support glsl)
      VertexBufferFormat format;                  // The format of the element data
      InputLayoutClassification input_slot_class; // A value that identifies the input data class for a single input. 
      s32 semantic_index;                                   // The semantic index for the element. A semantic index modifies a semantic, with an integer index number.
      s32 input_slot;                                       // An integer that identifies the input-assembler.
      s32 aligned_byte_offset;                              // This is optional. Offset, in bytes, to this element from the start of the vertex.
      s32 instance_data_step_rate;                          // The number of instances to draw using the same per-instance-data before advancing in the buffer by one element.
    };

    // A descriptor describing the input layout to construct
    struct InputLayoutDesc
    {
      rsl::vector<InputLayoutElementDesc> input_layout {};
    };

    // Base class for the input layout, only used as an interface
    class InputLayout
    {
      // Nothing to implement
    };
  } // namespace gfx
} // namespace rex