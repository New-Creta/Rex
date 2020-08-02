#pragma once

#include "rex_platform_global.h"

#include "core/input.h"

namespace rex
{
    namespace win32
    {
        class InputImpl : public rex::InputImpl
        {
        public:
            REX_PLATFORM_EXPORT bool isKeyPressed(KeyCode code) const override;
            REX_PLATFORM_EXPORT bool isKeyReleased(KeyCode code) const override;

            REX_PLATFORM_EXPORT bool isMouseButtonPressed(MouseCode button) const override;
            REX_PLATFORM_EXPORT bool isMouseButtonReleased(MouseCode button) const override;

            REX_PLATFORM_EXPORT Point getMousePosition() const override;

            REX_PLATFORM_EXPORT unsigned int getMouseX() const override;
            REX_PLATFORM_EXPORT unsigned int getMouseY() const override;
        };

        class Input : public rex::Input<rex::win32::InputImpl> 
        {

        };
    }
}