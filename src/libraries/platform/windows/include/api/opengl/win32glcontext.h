#include "rex_platform_global.h"

#include "core/context.h"

namespace rex
{
    namespace win32
    {
        class Context : public graphics::Context
        {
        public:
            Context(HWND hwnd);
            ~Context() final;

            REX_PLATFORM_EXPORT void resize(int width, int height) override;
            REX_PLATFORM_EXPORT void swapBuffers() override;

            REX_PLATFORM_EXPORT void* getNativeContext() const override;

            REX_PLATFORM_EXPORT static std::unique_ptr<Context> create(void* handle);
            REX_PLATFORM_EXPORT static void destroy(Context* context);
            
            REX_PLATFORM_EXPORT static void makeCurrent(Context* context);
            REX_PLATFORM_EXPORT static Context* getCurrent();

        private:
            bool create() override;
            bool destroy() override;

            void setAsCurrent() override;

            HWND m_hwnd;
            HDC m_hdc;
            HGLRC m_context;

            static Context* s_current;
            static bool s_glew_initialized;
        };
    }
}