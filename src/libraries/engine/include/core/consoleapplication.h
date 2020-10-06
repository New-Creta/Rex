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
            REX_ENGINE_EXPORT void appInitialize() final;
            REX_ENGINE_EXPORT void appUpdate(float dTime) final;
            REX_ENGINE_EXPORT void appQuit() final;

            virtual void onInitialize() {}
            virtual void onUpdate(float dTime) { UNUSED_PARAM(dTime); }
            virtual void onQuit() {}
        };
    }
}