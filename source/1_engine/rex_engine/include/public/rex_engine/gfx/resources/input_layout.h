#pragma once

#include "rex_engine/engine/types.h"
#include "rex_engine/gfx/core/input_layout_classification.h"
#include "rex_engine/gfx/core/vertex_buffer_format.h"
#include "rex_engine/gfx/shader_reflection/shader_param_reflection.h"
#include "rex_std/string_view.h"

namespace rex
{
  namespace gfx
  {
    // A semantic is a Direct X thing, which used to determine where to put vertex data
    enum class ShaderSemantic
    {
      // Shared between vertex and pixel shader
      Color,                  // Diffuse or specular color
      Position,               // Position of a vertex
      PSize,                  // Point size

      // Vertex shader
      BiNormal,               // Binormal
      BlendIndices,           // Binormal indices
      BlendWeight,            // Blend weights
      Normal,                 // Normal
      PositionT,              // Transformed vertex position
      Tangent,                // Tangent
      TexCoord,               // Texture coordinates

      // Pixel shader
      Fog,                    // Vertex fog
      TessFactor,             // Tesselation factor

      // Vertex shader
      InstanceMatrix,
      InstanceIndex
    };

    // Shader semantic are case sensitive, so we can't use reflection to get its name
    rsl::string_view shader_semantic_name(ShaderSemantic semantic);

    // This structure describe a single element in the input layout
    // Most members default initialization is fine and you only need to initialize semantic and the format
    struct InputLayoutElementDesc
    {
      ShaderSemantic semantic;                                                              // The semantic of the element, mainly used by DirectX
      VertexBufferFormat format;                                                            // The format of the element data
      InputLayoutClassification input_slot_class = InputLayoutClassification::PerVertex;    // A value that identifies the input data class for a single input. 
      s32 semantic_index = 0;                                                               // The semantic index for the element. A semantic index modifies a semantic, with an integer index number. eg NORMAL1
      s32 input_slot = 0;                                                                   // An integer that identifies the input-assembler.
      s32 aligned_byte_offset = -1;                                                         // This is optional. Offset, in bytes, to this element from the start of the vertex.
      s32 instance_data_step_rate = 0;                                                      // The number of instances to draw using the same per-instance-data before advancing in the buffer by one element.
    };

    // A descriptor describing the input layout to construct
    using InputLayoutDesc = rsl::vector<InputLayoutElementDesc>;
    bool operator==(const InputLayoutDesc& lhs, const InputLayoutDesc& rhs);
    bool operator!=(const InputLayoutDesc& lhs, const InputLayoutDesc& rhs);

    // Base class for the input layout, only used as an interface
    class InputLayout
    {
    public:
      InputLayout(s32 vertexSize, const InputLayoutDesc& desc);
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

namespace rsl
{
  inline namespace v1
  {
    template<>
    struct hash<rex::gfx::InputLayoutDesc>
    {
      hash_result operator()(const rex::gfx::InputLayoutDesc& desc) const
      {
        return rsl::crc32::compute(desc.data(), desc.size() * sizeof(desc[0]));
      }
    };
  }
}