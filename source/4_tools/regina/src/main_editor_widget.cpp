#include "regina/widgets/main_editor_widget.h"

#include "rex_engine/event_system/event_system.h"
#include "rex_engine/event_system/events/app/quit_app.h"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

namespace regina
{
	MainEditorWidget::MainEditorWidget()
		: m_show_imgui_demo(false)
	{}

	bool MainEditorWidget::on_update()
	{
		show_menu_bar();
	
		//ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
		////ImGuiID dockspace_id = ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

		//ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(300, 400));
		//ImGui::Begin("Dashboard");
		//ImGui::PopStyleVar();
		//ImGuiID dockspace_id = ImGui::GetID("DockSpace");
		//ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f));
		//static auto first_time = true;
		//if (first_time) {
		//	first_time = false;
		//	ImGui::DockBuilderRemoveNode(dockspace_id);
		//	ImGui::DockBuilderAddNode(dockspace_id);
		//	ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetMainViewport()->Size);

		//	auto dock_id_up = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Up, 0.5f, nullptr, &dockspace_id);
		//	auto dock_id_down = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Down, 0.5f, nullptr, &dockspace_id);
		//	ImGui::DockBuilderDockWindow("Up", dock_id_up);
		//	ImGui::DockBuilderDockWindow("Down", dock_id_down);

		//	ImGui::DockBuilderFinish(dockspace_id);
		//}
		//ImGui::End();

		ImGui::Begin("Up");
		ImGui::Text("Hello, up!");
		ImGui::End();

		ImGui::Begin("Down");
		ImGui::Text("Hello, down!");
		ImGui::End();

		//ImGuiIO& io = ImGui::GetIO();
		//ImGui::SetNextWindowPos(ImGui::GetWindowViewport()->Pos, ImGuiCond_Always);
		//ImGui::SetNextWindowSize(io.DisplaySize);

		//ImGuiWindowFlags window_flags{};
		//window_flags |= ImGuiWindowFlags_NoTitleBar;
		//window_flags |= ImGuiWindowFlags_NoScrollbar;
		//window_flags |= ImGuiWindowFlags_NoResize;
		//window_flags |= ImGuiWindowFlags_NoMove;
		//window_flags |= ImGuiWindowFlags_NoDocking;

		//// This is the main widget, all other widget should be docked into this
		//if (ImGui::Begin("Up", nullptr, window_flags))
		//{
		//	ImGui::Text("Hello up!");
		//}
		//ImGui::End();

		//if (ImGui::Begin("Down", nullptr, window_flags))
		//{
		//	ImGui::Text("Hello down!");

		//	//ImGuiID parent_node = ImGui::DockBuilderAddNode();
		//	//ImGui::Dock
		//}
		//ImGui::End();

		//if (m_show_imgui_demo)
		//{
		//	ImGui::ShowDemoWindow();
		//}

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

}