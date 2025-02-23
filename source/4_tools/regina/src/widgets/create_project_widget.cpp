#include "regina/widgets/create_project_widget.h"

#include "rex_engine/engine/types.h"

#include "imgui/imgui.h"

namespace regina
{
	CreateProjectWidget::CreateProjectWidget(rsl::function<void(rsl::string_view)>&& onCreateCallback)
		: m_on_create_callback(rsl::move(onCreateCallback))
	{}

	bool CreateProjectWidget::on_update()
	{
		bool should_close = false;

		// Center the window to the middle of the screen
		ImGuiIO& io = ImGui::GetIO();
		ImVec2 widgetPos = ImGui::GetWindowViewport()->Pos;
		widgetPos.x += io.DisplaySize.x * 0.5f;
		widgetPos.y += io.DisplaySize.y * 0.5f;
		ImGui::SetNextWindowPos(widgetPos, ImGuiCond_Always, ImVec2(0.5f, 0.5f));

		ImGui::Begin("Create New Project", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
		ImGui::InputText("Project Name", m_project_name.data(), m_project_name.max_size(), ImGuiInputTextFlags_EnterReturnsTrue);
		if (ImGui::Button("Create"))
		{
			m_project_name.reset_null_termination_offset();
			m_on_create_callback(m_project_name);
			should_close = true;
		}
		ImGui::End();

		return should_close;
	}
}
