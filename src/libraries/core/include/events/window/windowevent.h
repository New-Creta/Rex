#pragma once

#include "events/event.h"

namespace rex
{
	class Window;

	namespace events
	{

		class WindowEvent : public Event
		{
		public:
			EVENT_CLASS_CATEGORY(EventCategory::WINDOW);

			REX_CORE_EXPORT WindowEvent(rex::Window* window);
			REX_CORE_EXPORT ~WindowEvent() override;

			REX_CORE_EXPORT std::string toString() const override;

			template <typename T>
			const T& as() const;

		private:
			Window* m_window;
		};

		//-------------------------------------------------------------------------
		template <typename T>
		const T& WindowEvent::as() const
		{
			static_assert(std::is_base_of_v<WindowEvent, T>, "T is not derived from InputEvent");

			return static_cast<const T&>(*this);
		}
	}
}