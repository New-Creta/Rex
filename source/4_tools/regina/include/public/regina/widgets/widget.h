#pragma once

#include "rex_std/functional.h"
#include "rex_std/unordered_map.h"

namespace regina
{
	enum class WidgetEvent
	{
		Close
	};

	class Widget
	{
	public:
		virtual ~Widget() = default;

		// Returns true if widget should close
		bool update();
		void close();

		void set_callback(WidgetEvent evt, rsl::function<void()>&& callback);

	protected:
		// Returns true if widget should close
		virtual bool on_update() { return false; }
		virtual void on_close() {}

	private:
		rsl::unordered_map<WidgetEvent, rsl::function<void()>> m_event_callbacks;
	};
}