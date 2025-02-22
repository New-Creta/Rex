#pragma once

#include "regina//widgets/widget.h"
#include "rex_std/string_view.h"

namespace regina
{
	class CreateProjectWidget : public Widget
	{
	public:
		CreateProjectWidget(rsl::function<void(rsl::string_view)>&& onCreateCallback);
		
	protected:
		bool on_update() override;

	private:
		rsl::tiny_stack_string m_project_name;
		rsl::function<void(rsl::string_view)> m_on_create_callback;
	};
}