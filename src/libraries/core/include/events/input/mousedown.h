#pragma once

#include "rex_core_global.h"

#include "core/mousecodes.h"

#include "events/input/mouseevent.h"

#include "enums/yesnoenum.h"

namespace rex
{
	using IsDoubleClick = utility::YesNoEnum;

	namespace events
	{
		class MouseDown : public MouseEvent
		{
		public:
			EVENT_CLASS_TYPE(EventType::MOUSE_BUTTON_PRESSED);

			REX_CORE_EXPORT MouseDown(rex::Window* window, const MouseKeyMetaData& keyMetaData, MouseCode button, IsDoubleClick isDoubleClick, const MousePosition& mousePos);
			REX_CORE_EXPORT ~MouseDown() override;

			REX_CORE_EXPORT MouseCode getButton() const;
			REX_CORE_EXPORT IsDoubleClick isDoubleClick() const;

			REX_CORE_EXPORT std::string toString() const override;

		private:
			MouseCode m_button;
			IsDoubleClick m_is_double_click;
		};
	}
}