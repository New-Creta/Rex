#pragma once

#include "rex_opengl/log.h"
#include "rex_opengl/resources/buffer_layout.h"
#include "rex_opengl/resources/vertex.h"

namespace rex
{
  class InputAssembler
  {
  public:
    template <typename TVertexType>
    static void build();

  private:
    //-----------------------------------------------------------------------
    static void enable_vertex_attribute_array(const BufferLayout& bufferLayout);
  };

  //-----------------------------------------------------------------------
  template <typename TVertexType>
  void InputAssembler::build()
  {
    const auto& layouts = buffer_layouts();
    auto it             = layouts.find(typeid(TVertexType).hash_code());
    if(it == rsl::cend(layouts))
    {
      REX_ERROR(LogOpenGL, "\"Vertex Layout\" for vertex type: {0} was not found!", typeid(TVertexType).name());
      return;
    }

    const auto& layout = it->second;

    enable_vertex_attribute_array(layout);
  }
} // namespace rex