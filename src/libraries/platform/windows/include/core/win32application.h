#pragma once

#include "rex_platform_global.h"

#include "core/coreapplication.h"

namespace rex
{
    class Window;

    namespace win32
    {
        class Application : public CoreApplication
        {
        public:
            REX_PLATFORM_EXPORT Application();
            REX_PLATFORM_EXPORT ~Application() override;

            virtual rex::Window* getWindow() = 0;
            virtual const rex::Window* getWindow() const = 0;

        protected:
            virtual void appInitialize() = 0;
            virtual void appUpdate(float dTime) = 0;
            virtual void appQuit() = 0;

        private:
            REX_PLATFORM_EXPORT void platformInitialize() final;            // Functions need to be exported because they are protected in the base class.
            REX_PLATFORM_EXPORT void platformUpdate(float dTime) final;     // Functions need to be exported because they are protected in the base class.
            REX_PLATFORM_EXPORT void platformQuit() final;                  // Functions need to be exported because they are protected in the base class.
        };
    }
}