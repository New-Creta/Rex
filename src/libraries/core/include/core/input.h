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
        virtual void poll() = 0;

        virtual bool isKeyPressed(KeyCode code) const = 0;
        virtual bool isKeyDown(KeyCode code) const = 0;
        virtual bool isKeyReleased(KeyCode code) const = 0;
        virtual bool isKeyUp(KeyCode code) const = 0;

        virtual bool isMouseButtonPressed(MouseCode button) const = 0;
        virtual bool isMouseButtonDown(MouseCode button) const = 0;
        virtual bool isMouseButtonReleased(MouseCode button) const = 0;
        virtual bool isMouseButtonUp(MouseCode button) const = 0;

        virtual Point64 getMousePosition(bool previousFrame = false) const = 0;

        virtual int64 getMouseX(bool previousFrame = false) const = 0;
        virtual int64 getMouseY(bool previousFrame = false) const = 0;
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