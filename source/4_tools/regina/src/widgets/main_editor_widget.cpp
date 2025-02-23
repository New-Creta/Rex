#include "regina/widgets/main_editor_widget.h"

#include "regina/widgets/content_browser_widget.h"

#include "rex_engine/event_system/event_system.h"
#include "rex_engine/event_system/events/app/quit_app.h"

#include "rex_engine/gfx/imgui/imgui_utils.h"
#include "rex_engine/filesystem/path.h"
#include "rex_engine/filesystem/vfs.h"
#include "rex_engine/filesystem/file.h"
#include "rex_engine/filesystem/directory.h"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

namespace regina
{
	MainEditorWidget::MainEditorWidget()
		: m_show_imgui_demo(false)
		, m_show_imgui_style_editor(false)
	{
		//ImGuiIO& io = ImGui::GetIO();
		//if (!rex::file::exists(rsl::string_view(io.IniFilename)))
		//{
			rex::TempString main_layout_settings = rex::path::join(rex::vfs::root(), "Regina", "main_editor_layout.ini");
			ImGui::LoadIniSettingsFromDisk(main_layout_settings.data());
		//}

		m_widgets.emplace_back(rsl::make_unique<ContentBrowserWidget>());
	}

	bool MainEditorWidget::on_update()
	{
		render_menu_bar();
		render_docking_backpanel();

		if (m_show_imgui_demo)
		{
			ImGui::ShowDemoWindow();
		}
		if (m_show_imgui_style_editor)
		{
			rex::imgui::ScopedWidget widget("ImGui Style Editor");
			ImGui::ShowStyleEditor();
		}
	
		render_viewports();

		return false;
	}

	void MainEditorWidget::render_menu_bar()
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Quit"))
				{
					rex::event_system().enqueue_event(rex::QuitApp("Quit Pressed In Menu"));
				}
				if (ImGui::MenuItem("Show ImGui Demo", false, &m_show_imgui_demo))
				{
					m_show_imgui_demo != m_show_imgui_demo;
				}
				if (ImGui::MenuItem("Show ImGui Style Editor", false, &m_show_imgui_style_editor))
				{
					m_show_imgui_style_editor != m_show_imgui_style_editor;
				}

				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
	}
	void MainEditorWidget::render_docking_backpanel()
	{
		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;

		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
		// and handle the pass-thru hole, so we ask Begin() to not render a background.
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		{
			window_flags |= ImGuiWindowFlags_NoBackground;
		}

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Docking Backpanel", nullptr, window_flags);
		m_docking_backpanel = ImGui::GetCurrentWindow();
		ImGui::PopStyleVar();

		ImGui::PopStyleVar(2);

		// Submit the DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("Regina Dockspace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		ImGui::End();
	}
	void MainEditorWidget::render_viewports()
	{
		for (auto& widget : m_widgets)
		{
			widget->update();
		}

		ImGuiWindowFlags window_flags{};
		window_flags |= ImGuiWindowFlags_NoTitleBar;

		if (auto widget = rex::imgui::ScopedWidget("Viewport", nullptr, window_flags))
		{
			ImGui::GetCurrentWindow()->WindowClass.DockNodeFlagsOverrideSet |= ImGuiDockNodeFlags_HiddenTabBar;
			ImGui::Text("This is the viewport");
		}

		if (auto widget = rex::imgui::ScopedWidget("Scene Hierachy"))
		{
			ImGui::Text("This is the scene hiearchy");
		}

		if (auto widget = rex::imgui::ScopedWidget("Properties"))
		{
			ImGui::Text("This is the properties panel");
		}
	}
}