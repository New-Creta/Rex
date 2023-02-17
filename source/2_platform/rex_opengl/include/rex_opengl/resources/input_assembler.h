#pragma once

#include "rex_opengl/resources/buffer_layout.h"
#include "rex_opengl/resources/vertex.h"

#include "rex_engine/diagnostics/logging.h"

namespace rex
{
    class InputAssembler
    {
    public:
        template <typename T>
        static void build();

    private:
        //-----------------------------------------------------------------------
        static void enable_vertex_attribute_array(const BufferLayout& bufferLayout);
    };

    //-----------------------------------------------------------------------
    template<typename T>
    static void InputAssembler::build()
    {
        const auto& layouts = get_buffer_layouts();
        auto it = layouts.find(typeid(T).hash_code());
        if (it == rsl::cend(layouts))
        {
            REX_ERROR("\"Vertex Layout\" for vertex type: {0} was not found!", typeid(T).name());
            return;
        }

        const auto& layout = it->second;

        enable_vertex_attribute_array(layout);
    }
}