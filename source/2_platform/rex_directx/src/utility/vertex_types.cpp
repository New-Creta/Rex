#include "rex_directx/utility/vertex_types.h"

namespace rex
{
  namespace renderer
  {
    const D3D12_INPUT_LAYOUT_DESC XMVertexPosCol::input_layout = { XMVertexPosCol::input_elements, XMVertexPosCol::input_element_count };

    const D3D12_INPUT_ELEMENT_DESC XMVertexPosCol::input_elements[] = 
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
    };
  } // namespace renderer
} // namespace rex