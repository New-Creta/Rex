#pragma once

#include "rex_engine_global.h"

namespace rex
{
    namespace engine
    {
        class ConsoleApplication : public BaseApplication
        {
        public:
            REX_ENGINE_EXPORT ConsoleApplication();
            REX_ENGINE_EXPORT ~ConsoleApplication() override;

            rex::Window* getWindow() override;
            const rex::Window* getWindow() const override;

        protected:
            REX_ENGINE_EXPORT void appInitialize() override;
            REX_ENGINE_EXPORT void appUpdate(float dTime) override;
            REX_ENGINE_EXPORT void appQuit() override;
        };
    }
}