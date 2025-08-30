#include "regina/widgets/content_browser_widget.h"

#include "rex_engine/gfx/imgui/imgui_utils.h"
#include "rex_engine/gfx/imgui/imgui_scoped_style.h"
#include "rex_engine/gfx/imgui/imgui_scoped_widget.h"
#include "rex_engine/gfx/imgui/imgui_scoped_font.h"
#include "rex_engine/gfx/imgui/imgui_scoped_color.h"
#include "rex_engine/gfx/imgui/imgui_scoped_color_stack.h"
#include "rex_engine/gfx/imgui/imgui_colors.h"

#include "rex_engine/filesystem/path.h"
#include "rex_engine/filesystem/directory.h"
#include "rex_engine/filesystem/file.h"
#include "rex_engine/filesystem/vfs.h"
#include "rex_engine/system/open_file.h"
#include "rex_engine/profiling/profiling_session.h"

#include "rex_std/algorithm.h"
#include "rex_std/functional.h"
#include "rex_std/chrono.h"

namespace regina
{
	void Selection::add(ImGuiID id)
	{
		auto it = rsl::find(m_selected_items.cbegin(), m_selected_items.cend(), id);
		if (it == m_selected_items.cend())
		{
			add_without_search(id);
		}
	}
	void Selection::remove(ImGuiID id)
	{
		auto it = rsl::find(m_selected_items.cbegin(), m_selected_items.cend(), id);
		if (it != m_selected_items.cend())
		{
			remove_it(it);
		}
	}
	void Selection::toggle(ImGuiID id)
	{
		// TODO: put a contains func in rsl
		auto it = rsl::find(m_selected_items.cbegin(), m_selected_items.cend(), id);
		if (it == m_selected_items.cend())
		{
			add_without_search(id);
		}
		else
		{
			remove_it(it);
		}
	}
	void Selection::clear()
	{
		m_selected_items.clear();
	}
	bool Selection::is_selected(ImGuiID id) const
	{
		auto it = rsl::find(m_selected_items.cbegin(), m_selected_items.cend(), id);
		return it != m_selected_items.cend();
	}

	void Selection::add_without_search(ImGuiID id)
	{
		m_selected_items.push_back(id);
	}
	void Selection::remove_it(rsl::vector<ImGuiID>::const_iterator it)
	{
		m_selected_items.erase(it);
	}


	DEFINE_LOG_CATEGORY(LogContentBrowserWidget);

	ContentBrowserWidget::ContentBrowserWidget()
		: m_thumbnail_manager(rsl::make_unique<ThumbnailManager>())
	{
		change_directory(rex::vfs::instance()->root());
		m_hiearchy_items.reserve(m_directories_in_current_directory.size());
		for (rsl::string_view dir : m_directories_in_current_directory)
		{
			ContentBrowserHiearchyItem& hiearchy_item = m_hiearchy_items.emplace_back(dir);

			hiearchy_item.on_selected([this](rsl::string_view fullpath)
				{
					if (!rex::path::is_same(fullpath, m_current_directory))
					{
						change_directory(fullpath);
					}
				});
		}
	}

	void ContentBrowserWidget::on_open_in_editor(const open_in_editor_delegate& openInEditor)
	{
		m_open_in_editor_callback = openInEditor;
	}

	// We want a list of directories on the left hand side, similar to windows explorer
	// And a list of directories and files on the right hand side
	// The left hand side is 25% of the window, the right hand side is 75% of the window
	bool ContentBrowserWidget::on_update()
	{
		REX_PROFILE_FUNCTION();


		return false;
	}

	void ContentBrowserWidget::on_draw()
	{
		if (auto widget = rex::imgui::ScopedWidget("Content Browser", nullptr, ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar))
		{
			rex::imgui::ScopedStyle spacing(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 8.0f));
			rex::imgui::ScopedStyle padding(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 4.0f));
			rex::imgui::ScopedStyle cellPadding(ImGuiStyleVar_CellPadding, ImVec2(10.0f, 0.0f));

			ImGuiTableFlags table_flags = ImGuiTableFlags_Resizable
				| ImGuiTableFlags_SizingFixedFit
				| ImGuiTableFlags_BordersInnerV;

			s32 num_columns = 2;
			if (ImGui::BeginTable("Content Browser Table", num_columns, table_flags, ImVec2(0.0f, 0.0f)))
			{
				// Setup the the widgets of the columns
				// The hiearchy of all the directories is on the left
				// The content of the selected directory is on the right
				ImGui::TableSetupColumn("Hiearchy", 0, 300.0f);
				ImGui::TableSetupColumn("Directory Content", ImGuiTableColumnFlags_WidthStretch);

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);

				draw_content_hiearchy();
				ImGui::TableSetColumnIndex(1);
				draw_directory_content();
				
				ImGui::EndTable();
			}
		}
	}

	void ContentBrowserWidget::draw_content_hiearchy()
	{
		ImGui::BeginChild("##hiearchy");
		{
			rex::imgui::ScopedStyle spacing2(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
			rex::imgui::ScopedColourStack item_bg(
				ImGuiCol_Header, IM_COL32_DISABLE,
				ImGuiCol_HeaderActive, IM_COL32_DISABLE
			);

			for (ContentBrowserHiearchyItem& item : m_hiearchy_items)
			{
				item.update();
			}

			for (ContentBrowserHiearchyItem& item : m_hiearchy_items)
			{
				item.draw(m_current_directory);
			}
		}
		ImGui::EndChild();
	}
	void ContentBrowserWidget::draw_directory_content()
	{
		static f32 top_bar_height = 26.0f;
		static f32 bottom_bar_height = 32.0f;
		ImGui::BeginChild("##directory_content", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetWindowHeight() - top_bar_height - bottom_bar_height));
		{
			// Top bar
			{
				rex::imgui::ScopedStyle frame_border_size(ImGuiStyleVar_FrameBorderSize, 0.0f);
				draw_top_bar(top_bar_height);
			}

			ImGui::Separator();

			// Content

			ImGui::BeginChild("Scrolling");
			{
				rex::imgui::ScopedColor button_color(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
				rex::imgui::ScopedColor hovered_button_color(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 0.35f));

				if (ImGui::BeginPopupContextWindow(0, ImGuiPopupFlags_MouseButtonRight))
				{
					draw_content_structure_context_menu();
					ImGui::EndPopup();
				}

				const f32 paddingForOutline = 2.0f;
				const f32 scrollBarrOffset = 20.0f + ImGui::GetStyle().ScrollbarSize;
				f32 panelWidth = ImGui::GetContentRegionAvail().x - scrollBarrOffset;
				f32 cellSize = 100.0f; // EditorApplicationSettings::Get().ContentBrowserThumbnailSize + s_Padding + paddingForOutline;
				s32 columnCount = rsl::clamp_min((s32)(panelWidth / cellSize), 1);

				const f32 rowSpacing = 12.0f;
				rex::imgui::ScopedStyle spacing(ImGuiStyleVar_ItemSpacing, ImVec2(paddingForOutline, rowSpacing));
				ImGui::Columns(columnCount, 0, false);

				rex::imgui::ScopedStyle border(ImGuiStyleVar_FrameBorderSize, 0.0f);
				rex::imgui::ScopedStyle padding(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
				draw_items();

				//if (ImGui::IsWindowFocused() && !ImGui::IsMouseDragging(ImGuiMouseButton_Left))
				//{
					// UpdateInput();
				//}

				//RenderDeleteDialogue();
				//RenderNewScriptDialogue();
			}
			ImGui::EndChild();

			// Bottom bar
			draw_bottom_bar(bottom_bar_height);
		}
		ImGui::EndChild();
	}

	void ContentBrowserWidget::draw_content_structure_context_menu()
	{
		rex::imgui::ScopedStyle spacing(ImGuiStyleVar_ItemSpacing, ImVec2(4.0f, 4.0f));

		if (ImGui::BeginMenu("New"))
		{
			if (ImGui::MenuItem("Folder"))
			{
				REX_INFO(LogContentBrowserWidget, "Creating a new folder");

				//std::filesystem::path filepath = FileSystem::GetUniqueFileName(Project::GetActiveAssetDirectory() / m_CurrentDirectory->FilePath / "New Folder");

				//// NOTE(Peter): For some reason creating new directories through code doesn't trigger a file system change?
				//bool created = FileSystem::CreateDirectory(filepath);

				//if (created)
				//{
				//	Refresh();
				//	const auto& directoryInfo = GetDirectory(m_CurrentDirectory->FilePath / filepath.filename());
				//	size_t index = m_CurrentItems.FindItem(directoryInfo->Handle);
				//	if (index != ContentBrowserItemList::InvalidItem)
				//	{
				//		SelectionManager::DeselectAll(SelectionContext::ContentBrowser);
				//		SelectionManager::Select(SelectionContext::ContentBrowser, directoryInfo->Handle);
				//		m_CurrentItems[index]->StartRenaming();
				//	}
				//}
			}

			if (ImGui::MenuItem("Scene"))
			{
				REX_INFO(LogContentBrowserWidget, "Creating a new scene");
				//CreateAsset<Scene>("New Scene.hscene");
			}
			if (ImGui::BeginMenu("Physics"))
			{
				if (ImGui::MenuItem("Mesh Collider"))
				{
					REX_INFO(LogContentBrowserWidget, "Creating a mesh collider");
					//CreateAsset<MeshColliderAsset>("New Mesh Collider.hmc");
				}
				ImGui::EndMenu();
			}

			if (ImGui::MenuItem("Animation Graph"))
			{
				REX_INFO(LogContentBrowserWidget, "Creating a new animation graph");
				//auto extension = Project::GetEditorAssetManager()->GetDefaultExtensionForAssetType(AssetType::AnimationGraph);
				//auto animationGraphAsset = CreateAsset<AnimationGraphAsset>("New Animation Graph" + extension);
				//HZ_CORE_VERIFY(AnimationGraphAssetSerializer::TryLoadData("Resources/Animation/EmptyAnimationGraph" + extension, animationGraphAsset));
				//AssetImporter::Serialize(animationGraphAsset);
			}

			if (ImGui::MenuItem("Material"))
			{
				REX_INFO(LogContentBrowserWidget, "Creating a new material");
				//CreateAsset<MaterialAsset>("New Material.hmaterial");
			}
			if (ImGui::BeginMenu("Audio"))
			{
				if (ImGui::MenuItem("Sound Config"))
				{
					REX_INFO(LogContentBrowserWidget, "Creating a new sound config");
					//CreateAsset<SoundConfig>("New Sound Config.hsoundc");
				}
				if (ImGui::MenuItem("SoundGraph Sound"))
				{
					REX_INFO(LogContentBrowserWidget, "Creating a new sound graph");
					//CreateAsset<SoundGraphAsset>("New SoundGraph Sound.sound_graph");
				}
				ImGui::EndMenu();
			}

			if (ImGui::MenuItem("Script"))
			{
				REX_INFO(LogContentBrowserWidget, "Creating a new script");
				//s_OpenNewScriptPopup = true;
			}
			ImGui::EndMenu();
		}

		if (ImGui::MenuItem("Import"))
		{
			REX_INFO(LogContentBrowserWidget, "Importing a file");

			//std::filesystem::path filepath = FileSystem::OpenFileDialog();
			//if (!filepath.empty())
			//{
			//	FileSystem::CopyFile(filepath, Project::GetActiveAssetDirectory() / m_CurrentDirectory->FilePath);
			//	Refresh();
			//}
		}

		if (ImGui::MenuItem("Refresh"))
		{
			REX_INFO(LogContentBrowserWidget, "Refreshing");
			//Refresh();
		}
		if (ImGui::MenuItem("Copy", "Ctrl+C", nullptr, true /* = isEnabled*/))
		{
			REX_INFO(LogContentBrowserWidget, "Copying an item");
			//m_CopiedAssets.CopyFrom(SelectionManager::GetSelections(SelectionContext::ContentBrowser));
		}
		if (ImGui::MenuItem("Paste", "Ctrl+V", nullptr, true /* = isEnabled*/))
		{
			REX_INFO(LogContentBrowserWidget, "Pasting an item");
			//PasteCopiedAssets();
		}
		if (ImGui::MenuItem("Duplicate", "Ctrl+D", nullptr, true /* = isEnabled*/))
		{
			REX_INFO(LogContentBrowserWidget, "Duplicating an item");
			//m_CopiedAssets.CopyFrom(SelectionManager::GetSelections(SelectionContext::ContentBrowser));
			//PasteCopiedAssets();
		}

		ImGui::Separator();

		if (ImGui::MenuItem("Show in Explorer"))
		{
			REX_INFO(LogContentBrowserWidget, "Showing in explorer");
			//FileSystem::OpenDirectoryInExplorer(Project::GetActiveAssetDirectory() / m_CurrentDirectory->FilePath);
		}
	}

	void ContentBrowserWidget::draw_top_bar(f32 height)
	{
		ImGui::BeginChild("##top_bar", ImVec2(0, height));
		ImGui::BeginHorizontal("##top_bar", ImGui::GetWindowSize());
		{
			const f32 edgeOffset = 4.0f;

			// Navigation buttons
			{
				rex::imgui::ScopedStyle spacing(ImGuiStyleVar_ItemSpacing, ImVec2(2.0f, 0.0f));

				//auto contenBrowserButton = [height](const char* labelId, const Ref<Texture2D>& icon)
				//	{
				//		const ImU32 buttonCol = backgroundDark;
				//		const ImU32 buttonColP = rex::imgui::color_with_multiplied_value(backgroundDark, 0.8f);
				//		rex::imgui::ScopedColourStack buttonColours(ImGuiCol_Button, buttonCol,
				//			ImGuiCol_ButtonHovered, buttonCol,
				//			ImGuiCol_ButtonActive, buttonColP);

				//		const f32 iconSize = std::min(24.0f, height);
				//		const f32 iconPadding = 3.0f;
				//		const bool clicked = ImGui::Button(labelId, ImVec2(iconSize, iconSize));
				//		rex::imgui::DrawButtonImage(icon, rex::imgui::textDarker,
				//			rex::imgui::ColourWithMultipliedValue(rex::imgui::textDarker, 1.2f),
				//			rex::imgui::ColourWithMultipliedValue(rex::imgui::textDarker, 0.8f),
				//			rex::imgui::RectExpanded(rex::imgui::Getitem_rect(), -iconPadding, -iconPadding));

				//		return clicked;
				//	};

				ImGui::Text("Navigation buttons go here");

				//if (contenBrowserButton("##back", EditorResources::BackIcon))
				//{
				//	//OnBrowseBack();
				//}
				rex::imgui::set_tooltip("Previous directory");

				ImGui::Spring(-1.0f, edgeOffset);

				//if (contenBrowserButton("##forward", EditorResources::ForwardIcon))
				//{
				//	//OnBrowseForward();
				//}
				rex::imgui::set_tooltip("Next directory");

				ImGui::Spring(-1.0f, edgeOffset * 2.0f);

				//if (contenBrowserButton("##refresh", EditorResources::RefreshIcon))
				//{
				//	Refresh();
				//}
				//rex::imgui::set_tooltip("Refresh");
				//if (contenBrowserButton("##clearThumbnailCache", EditorResources::ClearIcon))
				//{
				//	m_ThumbnailCache->Clear();
				//}
				//rex::imgui::set_tooltip("Clear thumbnail cache");

				ImGui::Spring(-1.0f, edgeOffset * 2.0f);
			}

			// Search
			{
				rex::imgui::shift_cursor_y(2.0f);
				ImGui::SetNextItemWidth(200);

				//if (s_ActivateSearchWidget)
				//{
				//	ImGui::SetKeyboardFocusHere();
				//	s_ActivateSearchWidget = false;
				//}

				//if (rex::imgui::Widgets::SearchWidget<MAX_INPUT_BUFFER_LENGTH>(m_SearchBuffer))
				//{
				//	if (strlen(m_SearchBuffer) == 0)
				//	{
				//		ChangeDirectory(m_CurrentDirectory);
				//	}
				//	else
				//	{
				//		m_CurrentItems = Search(m_SearchBuffer, m_CurrentDirectory);
				//		SortItemList();
				//	}
				//}
				rex::imgui::shift_cursor_y(-2.0f);
			}

			//if (m_UpdateNavigationPath)
			//{
			//	m_BreadCrumbData.clear();

			//	Ref<DirectoryInfo> current = m_CurrentDirectory;
			//	while (current && current->Parent != nullptr)
			//	{
			//		m_BreadCrumbData.push_back(current);
			//		current = current->Parent;
			//	}

			//	std::reverse(m_BreadCrumbData.begin(), m_BreadCrumbData.end());
			//	m_UpdateNavigationPath = false;
			//}

			// Breadcrumbs
			{
				rex::imgui::ScopedFont boldFont(ImGui::GetIO().Fonts->Fonts[0]);
				rex::imgui::ScopedColor textColour(ImGuiCol_Text, rex::imgui::colors::text_darker);

				rsl::string_view assetsDirectoryName = rex::path::cwd(); //  m_Project->GetConfig().AssetDirectory;
				ImVec2 textSize = ImGui::CalcTextSize(assetsDirectoryName.data());
				const f32 textPadding = ImGui::GetStyle().FramePadding.y;
				//if (ImGui::Selectable(assetsDirectoryName.c_str(), false, 0, ImVec2(textSize.x, textSize.y + textPadding)))
				//{
				//	SelectionManager::DeselectAll(SelectionContext::ContentBrowser);
				//	ChangeDirectory(m_BaseDirectory);
				//}
				//UpdateDropArea(m_BaseDirectory);

			//	for (auto& directory : m_BreadCrumbData)
			//	{
			//		ImGui::Text("/");

			//		std::string directoryName = directory->FilePath.filename().string();
			//		ImVec2 textSize = ImGui::CalcTextSize(directoryName.c_str());
			//		if (ImGui::Selectable(directoryName.c_str(), false, 0, ImVec2(textSize.x, textSize.y + textPadding)))
			//		{
			//			SelectionManager::DeselectAll(SelectionContext::ContentBrowser);
			//			ChangeDirectory(directory);
			//		}

			//		UpdateDropArea(directory);
			//	}
			//}

			// Settings button
				ImGui::Spring();
				//if (rex::imgui::Widgets::OptionsButton())
				//{
				//	ImGui::OpenPopup("ContentBrowserSettings");
				//}
				//rex::imgui::set_tooltip("Content Browser settings");


				//if (rex::imgui::BeginPopup("ContentBrowserSettings"))
				//{
				//	auto& editorSettings = EditorApplicationSettings::Get();

				//	bool saveSettings = ImGui::MenuItem("Show Asset Types", nullptr, &editorSettings.ContentBrowserShowAssetTypes);
				//	saveSettings |= ImGui::SliderInt("##thumbnail_size", &editorSettings.ContentBrowserThumbnailSize, 96, 512);
				//	rex::imgui::set_tooltip("Thumnail Size");

				//	if (saveSettings)
				//		EditorApplicationSettingsSerializer::SaveSettings();

				//	rex::imgui::EndPopup();
				//}

			}
			ImGui::EndHorizontal();
			ImGui::EndChild();
		}
	}
	void ContentBrowserWidget::draw_bottom_bar(f32 height)
	{
		// Display the full filepath of the current selected item as well as its size and other metadata
	}
	void ContentBrowserWidget::draw_items()
	{
		//m_IsAnyItemHovered = false;

		f32 edgeOffset = 6.0f;

		f32 textLineHeight = ImGui::GetTextLineHeightWithSpacing() + edgeOffset * 2.0f;
		f32 thumbnailSize = 128.0f;
		f32 thumbBhWidth = 102.0f;
		f32 thumbBhHeight = 102.0f;
		f32 infoPanelWidth = thumbnailSize;
		f32 infoPanelHeight = textLineHeight;
		f32 itemSpacing = 1.0f;

		// TODO(Peter): This method of handling actions isn't great... It's starting to become spaghetti...
		rex::temp_string fullpath;
		const ContentBrowserItem* selected_item = nullptr;
		for (ContentBrowserItem& item : m_current_items)
		{
			// Each item should look something like this
			// +------------------------------+
			// |                              |
			// |                              |
			// |                              |
			// |           THUMBNAIL          |             
			// |                              |
			// |                              |
			// |                              |
			// +------------------------------+
			// |           FILENAME           |                   
			// +------------------------------+

			if (item.draw())
			{
				selected_item = &item;
			}
			ImGui::NextColumn();
		}

		if (selected_item)
		{
			m_open_in_editor_callback(rex::path::join(m_current_directory, selected_item->path()));
		}
	}

	const Thumbnail* ContentBrowserWidget::thumbnail_for_path(rsl::string_view path) const
	{
		if (m_thumbnail_manager->has_thumbnail(path))
		{
			return m_thumbnail_manager->thumbnail_for_path(path);
		}

		if (rex::directory::exists(path))
		{
			return m_thumbnail_manager->directory_thumbnail();
		}
		else if (rex::file::exists(path))
		{
			return m_thumbnail_manager->file_thumbnail();
		}
		else
		{
			return m_thumbnail_manager->unknown_thumbnail();
		}
	}

	void ContentBrowserWidget::change_directory(rsl::string_view newDirectory)
	{
		REX_INFO(LogContentBrowserWidget, "Changing dir {} -> {}", m_current_directory, newDirectory);

		m_current_directory.assign(newDirectory);
		m_files_in_current_directory = rex::directory::list_files(m_current_directory);
		m_directories_in_current_directory = rex::directory::list_dirs(m_current_directory);

		rsl::sort(m_files_in_current_directory.begin(), m_files_in_current_directory.end());
		rsl::sort(m_directories_in_current_directory.begin(), m_directories_in_current_directory.end());

		m_current_items.clear();
		m_current_items.reserve(m_directories_in_current_directory.size() + m_files_in_current_directory.size());
		for (rsl::string_view dir : m_directories_in_current_directory)
		{
			m_current_items.emplace_back(dir, thumbnail_for_path(dir));
		}
		for (rsl::string_view file : m_files_in_current_directory)
		{
			m_current_items.emplace_back(file, thumbnail_for_path(file));
		}
	}
}