#pragma once

#include "rex_platform_global.h"

#include "core/input.h"

namespace rex
{
    namespace win32
    {
        class InputController;

        class InputImpl : public rex::InputImpl
        {
        public:
            REX_PLATFORM_EXPORT InputImpl();
            REX_PLATFORM_EXPORT ~InputImpl();

            REX_PLATFORM_EXPORT void poll() override;

            REX_PLATFORM_EXPORT bool isKeyPressed(KeyCode code) const override;
            REX_PLATFORM_EXPORT bool isKeyDown(KeyCode code) const override;
            REX_PLATFORM_EXPORT bool isKeyReleased(KeyCode code) const override;
            REX_PLATFORM_EXPORT bool isKeyUp(KeyCode code) const override;

            REX_PLATFORM_EXPORT bool isMouseButtonPressed(MouseCode button) const override;
            REX_PLATFORM_EXPORT bool isMouseButtonDown(MouseCode button) const override;
            REX_PLATFORM_EXPORT bool isMouseButtonReleased(MouseCode button) const override;
            REX_PLATFORM_EXPORT bool isMouseButtonUp(MouseCode button) const override;

            REX_PLATFORM_EXPORT Point64 getMousePosition(bool previousFrame = false) const override;

            REX_PLATFORM_EXPORT int64 getMouseX(bool previousFrame = false) const override;
            REX_PLATFORM_EXPORT int64 getMouseY(bool previousFrame = false) const override;

        private:
            std::unique_ptr<InputController> m_controller;
        };

        class Input : public rex::Input<rex::win32::InputImpl> 
        {

        };
    }
}