#pragma once

#include "core_context.h"

namespace rex
{
    namespace opengl
    {
        class OpenGLContext : public IGraphicsContext
        {
        public:
            OpenGLContext(const WindowHandle& hwnd);
            ~OpenGLContext() override = default;

            void resize(int32 x, int32 y, int32 width, int32 height) override;
            void make_current() override;
            void present() override;

        private:
            WindowHandle m_handle;
        };
    }
}