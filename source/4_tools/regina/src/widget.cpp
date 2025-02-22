#include "regina/widgets/widget.h"

namespace regina
{
	bool Widget::update()
	{
		return on_update();
	}

	void Widget::close()
	{
		on_close();

		if (m_event_callbacks.contains(WidgetEvent::Close))
		{
			m_event_callbacks.at(WidgetEvent::Close)();
		}
	}

	void Widget::set_callback(WidgetEvent evt, rsl::function<void()>&& callback)
	{
		m_event_callbacks[evt] = rsl::move(callback);
	}
}