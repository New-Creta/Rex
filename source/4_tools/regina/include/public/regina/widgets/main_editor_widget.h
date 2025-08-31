#pragma once

#include "regina/widgets/widget.h"
#include "regina/widgets/viewport_widget.h"

#include "regina/scene_manager.h"
#include "regina/world_composer.h"
#include "rex_engine/assets/map.h"
#include "rex_engine/assets/tilemap.h"
#include "rex_engine/assets/tileset_asset.h"

#include "rex_std/bonus/math/point.h"
#include "rex_std/vector.h"

struct ImGuiWindow;

namespace regina
{
	class MainEditorWidget : public Widget
	{
	public:
		MainEditorWidget();

		void set_active_map(rex::Map* map);

	protected:
		bool on_update() override;
		void on_draw() override;

	private:
		// Initializing
		void init_widget_creators();
		void add_default_widgets();

		// Updating
		void update_widgets();

		// Drawing
		void draw_menu_bar();
		void draw_docking_backpanel();
		void draw_widgets();
		void draw_imgui_widgets();

		// Events
		void on_new_active_map();

		template <typename T>
		void create_widget()
		{
			create_widget(rsl::type_id<T>());
		}
		void create_widget(rsl::type_id_t typeId);

	private:
		// UI
		ImGuiWindow* m_docking_backpanel;
		rsl::vector<rsl::unique_ptr<Widget>> m_widgets;
		rsl::unordered_map<rsl::type_id_t, rsl::function<void()>> m_widget_create_funcs;

		// Scene/World 
		rex::Map* m_active_map;
		WorldComposer m_world_composer;
		Viewport* m_viewport;

		// ImGui flags
		bool m_show_imgui_demo;
		bool m_show_imgui_style_editor;
	};
}