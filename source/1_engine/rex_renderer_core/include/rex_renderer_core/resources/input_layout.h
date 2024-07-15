#pragma once

#include "rex_engine/engine/types.h"
#include "rex_renderer_core/gfx/input_layout_classification.h"
#include "rex_renderer_core/gfx/vertex_buffer_format.h"
#include "rex_renderer_core/shader_reflection/shader_param_reflection.h"
#include "rex_std/string_view.h"

namespace rex
{
  namespace gfx
  {
    struct InputLayoutElementDesc
    {
      rsl::string_view semantic_name;                       // The HLSL semantic name associated with the element (will change when we support glsl)
      VertexBufferFormat format;                            // The format of the element data
      InputLayoutClassification input_slot_class;           // A value that identifies the input data class for a single input. 
      s32 semantic_index;                                   // The semantic index for the element. A semantic index modifies a semantic, with an integer index number. eg NORMAL1
      s32 input_slot;                                       // An integer that identifies the input-assembler.
      s32 aligned_byte_offset;                              // This is optional. Offset, in bytes, to this element from the start of the vertex.
      s32 instance_data_step_rate;                          // The number of instances to draw using the same per-instance-data before advancing in the buffer by one element.
    };

    // A descriptor describing the input layout to construct
    struct InputLayoutDesc
    {
      rsl::vector<InputLayoutElementDesc> input_layout {};
    };

    InputLayoutDesc create_input_layout_desc_from_reflection(const rsl::vector<ShaderParamReflection>& shaderInputParams);

    // Base class for the input layout, only used as an interface
    class InputLayout
    {
    public:
      InputLayout(s32 vertexSize)
        : m_vertex_size(vertexSize)
      {}

      s32 vertex_size() const
      {
        return m_vertex_size;
      }

      virtual bool validate_desc(const InputLayoutDesc& desc) = 0;

    private:
      s32 m_vertex_size;
    };
  } // namespace gfx
} // namespace rex