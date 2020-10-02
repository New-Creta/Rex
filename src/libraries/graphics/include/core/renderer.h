#pragma once

namespace rex
{
    namespace graphics
    {
        enum class RendererAPI
        {
            NONE = 0, 
            OPENGL = 1
        };

        class Renderer
        {
        public:
            inline static RendererAPI getAPI() { return s_renderer_api; }

        private:
            static RendererAPI s_renderer_api;
        };
    }
}