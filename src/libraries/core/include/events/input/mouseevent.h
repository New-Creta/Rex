#pragma once

#include "math/point.h"

#include "events/input/inputevent.h"

namespace rex
{
	struct MousePosition
	{
		Point screen_position;
		Point local_position;
	};
    
    namespace events
    {
    	struct MouseKeyMetaData
    	{
        	bool is_ctrl_down;
        	bool is_shift_down;
    	};

        class MouseEvent : public InputEvent
        {
        public:
            EVENT_CLASS_CATEGORY(EventCategory::INPUT | EventCategory::MOUSE);

            ~MouseEvent() override;

            REX_CORE_EXPORT const Point& getScreenPosition() const;
            REX_CORE_EXPORT const Point& getLocalPosition() const;

            REX_CORE_EXPORT bool isCtrlDown() const;
            REX_CORE_EXPORT bool isShiftDown() const;


        protected:
            MouseEvent(rex::Window* window, const MouseKeyMetaData& keyMetaData, const MousePosition& mousPos);

        private:
            MousePosition m_mouse_position;
            bool m_is_ctrl_down;
            bool m_is_shift_down;
        };
    }
}