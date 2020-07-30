#ifdef _OPENGL

#include "rex_platform_global.h"

#include "core/glcontext.h"

namespace rex
{
    namespace win32
    {
        class Context : public gl::Context
        {
        public:
            REX_PLATFORM_EXPORT Context(const gl::DriverData& data);
            REX_PLATFORM_EXPORT ~Context() override;

        protected:
            REX_PLATFORM_EXPORT void* createLegacyContext() override;
            REX_PLATFORM_EXPORT void* createModernContext() override;

            REX_PLATFORM_EXPORT void makeCurrent(void* context) override;
            REX_PLATFORM_EXPORT void deleteContext(void* context) override;

        private:
            void* initializeTemporaryContext();
        };
    }
}

#endif