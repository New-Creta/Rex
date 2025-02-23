#include "regina/widgets/main_editor_widget.h"

#include "rex_engine/event_system/event_system.h"
#include "rex_engine/event_system/events/app/quit_app.h"

#include "rex_engine/gfx/imgui/imgui_utils.h"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

namespace regina
{
	MainEditorWidget::MainEditorWidget()
		: m_show_imgui_demo(false)
	{
		init_widgets();
	}

	bool MainEditorWidget::on_update()
	{
		show_menu_bar();
	
		ImGuiWindowFlags window_flags{};
		//window_flags |= ImGuiWindowFlags_NoTitleBar;
		window_flags |= ImGuiWindowFlags_NoResize;
		//window_flags |= ImGuiWindowFlags_NoMove;
		//window_flags |= ImGuiWindowFlags_NoScrollbar;

		ImGui::Begin("Up", nullptr, window_flags);
		ImGui::Text("Hello, up!");
		ImGui::End();

		ImGui::Begin("Down", nullptr, window_flags);
		ImGui::Text("Hello, down!");
		ImGui::End();

		if (m_show_imgui_demo)
		{
			ImGui::ShowDemoWindow();
		}

		return false;
	}

	void MainEditorWidget::show_menu_bar()
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Quit"))
				{
					rex::event_system().enqueue_event(rex::QuitApp("Quit Pressed In Menu"));
				}
				if (ImGui::MenuItem("Enable ImGui Demo", false, &m_show_imgui_demo))
				{
					m_show_imgui_demo != m_show_imgui_demo;
				}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
	}
	void MainEditorWidget::init_widgets()
	{
		ImGuiIO& io = ImGui::GetIO();
		ImGui::SetNextWindowPos(ImGui::GetWindowViewport()->Pos);
		ImGui::SetNextWindowSize(io.DisplaySize);

		ImGui::Begin("Up");
		ImGui::Text("Hello, up!");
		ImGui::End();

		ImGui::Begin("Down");
		ImGui::Text("Hello, down!");
		ImGui::End();

		rex::gfx::dock_to_window("Down", "Up", ImGuiDir_None);
	}
}