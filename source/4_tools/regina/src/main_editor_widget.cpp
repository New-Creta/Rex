#include "regina/widgets/main_editor_widget.h"

#include "rex_engine/event_system/event_system.h"
#include "rex_engine/event_system/events/app/quit_app.h"

#include "imgui/imgui.h"

namespace regina
{
	bool MainEditorWidget::on_update()
	{
		show_menu_bar();

		ImGui::ShowDemoWindow();

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
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
	}

}