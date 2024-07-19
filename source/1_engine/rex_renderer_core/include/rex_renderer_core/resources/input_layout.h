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
      rsl::tiny_stack_string semantic_name;                 // The HLSL semantic name associated with the element (will change when we support glsl)
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

    // Using shader reflection's input parameters, create an input layout description.
    InputLayoutDesc create_input_layout_desc_from_reflection(const rsl::vector<ShaderParamReflection>& shaderInputParams);

    // Base class for the input layout, only used as an interface
    class InputLayout
    {
    public:
      InputLayout(s32 vertexSize, InputLayoutDesc&& desc);
      virtual ~InputLayout() = default;

      // As an input layout stores data per vertex (at least at the moment)
      // We can store the size of a single vertex using the input layout
      s32 vertex_size() const;

      // Validate an description to see if it can be used with this input layout
      // It's possible some elements do not match directly but can be converted
      // eg: an 4 component normalized byte type can be converted to a 4 component float type
      bool validate_desc(const InputLayoutDesc& desc);

    private:
      s32 m_vertex_size;
      InputLayoutDesc m_desc;
    };
  } // namespace gfx
} // namespace rex