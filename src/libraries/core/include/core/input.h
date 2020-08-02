#pragma once

#include "core/keycodes.h"
#include "core/mousecodes.h"

#include "math/point.h"

#include "patterns/singleton.h"

namespace rex
{
    class InputImpl
    {
    public:
        virtual bool isKeyPressed(KeyCode code) const = 0;
        virtual bool isKeyReleased(KeyCode code) const = 0;

        virtual bool isMouseButtonPressed(MouseCode button) const = 0;
        virtual bool isMouseButtonReleased(MouseCode button) const = 0;

        virtual Point getMousePosition() const = 0;

        virtual unsigned int getMouseX() const = 0;
        virtual unsigned int getMouseY() const = 0;
    };

    template <typename T>
    class Input : public Singleton<T>
    {
        static_assert(std::is_base_of_v<InputImpl, T>, "Invalid event for event dispatcher, T not derived of InputImpl");

    protected:
        Input() = default;
        ~Input() override = default;
    };
}