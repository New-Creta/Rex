#pragma once

#include "rex_engine_global.h"

#include "core/applicationconfig.h"

namespace rex
{
    namespace engine
    {
        class ConsoleApplication : public BaseApplication
        {
        public:
            REX_ENGINE_EXPORT ConsoleApplication();
            REX_ENGINE_EXPORT ~ConsoleApplication() override;

        protected:
            REX_ENGINE_EXPORT void appInitialize() override;
            REX_ENGINE_EXPORT void appUpdate(float dTime) override;
            REX_ENGINE_EXPORT void appQuit() override;
        };
    }
}