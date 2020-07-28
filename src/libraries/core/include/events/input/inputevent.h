#pragma once

#include "events/event.h"

namespace rex
{
	class Window;

	namespace events
	{
		class InputEvent : public Event
		{
		public:
			~InputEvent() override;

			REX_CORE_EXPORT std::string toString() const override;

			template <typename T>
			const T& as() const;

		protected:
			InputEvent(rex::Window* window);

		private:
			Window* m_window;
		};

		//-------------------------------------------------------------------------
		template <typename T>
		const T& InputEvent::as() const
		{
			static_assert(std::is_base_of_v<InputEvent, T>, "T is not derived from InputEvent");

			return static_cast<const T&>(*this);
		}
	}
}