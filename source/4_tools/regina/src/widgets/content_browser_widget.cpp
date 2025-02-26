#include "regina/widgets/content_browser_widget.h"

#include "rex_engine/gfx/imgui/imgui_utils.h"

#include "rex_engine/filesystem/path.h"
#include "rex_engine/filesystem/directory.h"
#include "rex_engine/filesystem/file.h"
#include "rex_engine/filesystem/vfs.h"

#include "rex_std/algorithm.h"
#include "rex_std/functional.h"

namespace regina
{


	DEFINE_LOG_CATEGORY(LogContentBrowserWidget);

	ContentBrowserWidget::ContentBrowserWidget()
	{
		m_current_directory.assign(rex::vfs::root());
		m_files_in_current_directory = rex::directory::list_files(m_current_directory);
		m_directories_in_current_directory = rex::directory::list_dirs(m_current_directory);

		rsl::sort(m_files_in_current_directory.begin(), m_files_in_current_directory.end());
		rsl::sort(m_directories_in_current_directory.begin(), m_directories_in_current_directory.end());
	}

	// We want a list of directories on the left hand side, similar to windows explorer
	// And a list of directories and files on the right hand side
	// The left hand side is 25% of the window, the right hand side is 75% of the window
	bool ContentBrowserWidget::on_update()
	{
		// Create the left table

		// Create the right table

		if (auto widget = rex::imgui::ScopedWidget("Content Browser"))
		{
			ImGuiTableFlags table_flags = ImGuiTableFlags_Resizable
				| ImGuiTableFlags_SizingFixedFit
				| ImGuiTableFlags_BordersInnerV;

			//const rsl::vector<rsl::string>& assets = dummy_content.at(m_current_directory);

			if (ImGui::BeginTable("Assets", 2, table_flags))
			{
				ImGui::TableSetupColumn("Outliner", 0, 300.0f);
				ImGui::TableSetupColumn("Directory Structure", ImGuiTableColumnFlags_WidthStretch);

				ImGui::TableNextRow();

				ImGui::BeginChild("##hiearchy");
				{
					//for (rsl::string_view directory : dummy_dirs)
					//{
						//ImGui::TreeNode(directory.data());

					rex::imgui::ScopedStyle spacing(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
					rex::imgui::ScopedColourStack item_bg(ImGuiCol_Header, IM_COL32_DISABLE,
						ImGuiCol_HeaderActive, IM_COL32_DISABLE);

					//if (m_BaseDirectory)
					//{
						// TODO(Yan): can we not sort this every frame?
						//std::vector<Ref<DirectoryInfo>> directories;
						//directories.reserve(m_BaseDirectory->SubDirectories.size());
						//for (auto& [handle, directory] : m_BaseDirectory->SubDirectories)
						//	directories.emplace_back(directory);

						//rsl::sort(directories.begin(), directories.end(), [](const auto& a, const auto& b)
						//	{
						//		return a->FilePath.stem().string() < b->FilePath.stem().string();
						//	});

					for (rsl::string_view directory : m_directories_in_current_directory)
					{
						render_directory_hiearchy(directory);
					}
					//}

					// Draw side shadow
					//ImRect windowRect = rex::imgui::RectExpanded(ImGui::GetCurrentWindow()->Rect(), 0.0f, 10.0f);
					//ImGui::PushClipRect(windowRect.Min, windowRect.Max, false);
					//rex::imgui::DrawShadowInner(EditorResources::ShadowTexture, 20, windowRect, 1.0f, windowRect.GetHeight() / 4.0f, false, true, false, false);
					//ImGui::PopClipRect();
				//}
				}
				ImGui::EndChild();

				ImGui::TableSetColumnIndex(1);

				const float topBarHeight = 26.0f;
				const float bottomBarHeight = 32.0f;
				ImGui::BeginChild("##directory_content", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetWindowHeight() - topBarHeight - bottomBarHeight));
				{
					ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
					render_top_bar(topBarHeight);
					ImGui::PopStyleVar();

					ImGui::Separator();

					ImGui::BeginChild("Scrolling");
					{
						ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
						ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 0.35f));

						ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4.0f, 4.0f));
						if (ImGui::BeginPopupContextWindow(0, ImGuiPopupFlags_MouseButtonRight))
						{
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
							ImGui::EndPopup();
						}
						ImGui::PopStyleVar(); // ItemSpacing

						const float paddingForOutline = 2.0f;
						const float scrollBarrOffset = 20.0f + ImGui::GetStyle().ScrollbarSize;
						float panelWidth = ImGui::GetContentRegionAvail().x - scrollBarrOffset;
						float cellSize = 100.0f; // EditorApplicationSettings::Get().ContentBrowserThumbnailSize + s_Padding + paddingForOutline;
						int columnCount = (int)(panelWidth / cellSize);
						if (columnCount < 1) columnCount = 1;

						{
							const float rowSpacing = 12.0f;
							rex::imgui::ScopedStyle spacing(ImGuiStyleVar_ItemSpacing, ImVec2(paddingForOutline, rowSpacing));
							ImGui::Columns(columnCount, 0, false);

							rex::imgui::ScopedStyle border(ImGuiStyleVar_FrameBorderSize, 0.0f);
							rex::imgui::ScopedStyle padding(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
							render_items();
						}

						if (ImGui::IsWindowFocused() && !ImGui::IsMouseDragging(ImGuiMouseButton_Left))
						{
							// UpdateInput();
						}

						ImGui::PopStyleColor(2);

						//RenderDeleteDialogue();
						//RenderNewScriptDialogue();
					}
					ImGui::EndChild();
				}
				ImGui::EndChild();

				ImGui::EndTable();
			}
		}

		return false;
	}

	void ContentBrowserWidget::render_directory_hiearchy(rsl::string_view directory)
	{
		rsl::string_view directory_name = rex::path::filename(directory);

		rsl::string name(directory_name);
		rsl::string id = name + "_TreeNode";
		bool previousState = ImGui::TreeNodeBehaviorIsOpen(ImGui::GetID(id.c_str()));
		previousState = ImGui::TreeNodeBehaviorIsOpen(ImGui::GetID(name.c_str()));

		// ImGui item height hack
		auto* window = ImGui::GetCurrentWindow();
		window->DC.CurrLineSize.y = 20.0f; // This causes issues where the node item doesn't want to open anymore
		window->DC.CurrLineTextBaseOffset = 3.0f;
		//---------------------------------------------

		const ImRect itemRect = { 
			window->WorkRect.Min.x, window->DC.CursorPos.y,
			window->WorkRect.Max.x, window->DC.CursorPos.y + window->DC.CurrLineSize.y };

		const bool isItemClicked = [&itemRect, &id]
			{
				return false;

				if (ImGui::ItemHoverable(itemRect, ImGui::GetID(id.c_str()), ImGuiItemFlags_None))
				{
					return ImGui::IsMouseDown(ImGuiMouseButton_Left) || ImGui::IsMouseReleased(ImGuiMouseButton_Left);
				}
				return false;
			}();

		const bool isWindowFocused = ImGui::IsWindowFocused();

		auto fillWithColour = [&](const ImColor& colour)
			{
				const ImU32 bgColour = ImGui::ColorConvertFloat4ToU32(colour);
				ImGui::GetWindowDrawList()->AddRectFilled(itemRect.Min, itemRect.Max, bgColour);
			};

		// Fill with light selection colour if any of the child entities selected
		auto checkIfAnyDescendantSelected = [&](rsl::string_view directory, auto isAnyDescendantSelected) -> bool
			{
				if (rex::path::is_same(directory, m_current_directory))
				{
					return true;
				}

				//if (!directory->SubDirectories.empty())
				//{
				//	for (auto& [childHandle, childDir] : directory->SubDirectories)
				//	{
				//		if (isAnyDescendantSelected(childDir, isAnyDescendantSelected))
				//			return true;
				//	}
				//}

				return false;
			};

		const bool isAnyDescendantSelected = checkIfAnyDescendantSelected(directory, checkIfAnyDescendantSelected);
		const bool isActiveDirectory = rex::path::is_same(directory, m_current_directory);

		ImGuiTreeNodeFlags flags = (isActiveDirectory ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_SpanFullWidth;

		// Fill background
		//----------------
		if (isActiveDirectory || isItemClicked)
		{
			if (isWindowFocused)
			{
				fillWithColour(rex::imgui::selection);
			}
			else
			{
				const ImColor col = rex::imgui::color_with_multiplied_value(rex::imgui::selection, 0.8f);
				fillWithColour(rex::imgui::color_with_multiplied_value(col, 0.7f));
			}

			ImGui::PushStyleColor(ImGuiCol_Text, rex::imgui::backgroundDark);
		}
		else if (isAnyDescendantSelected)
		{
			fillWithColour(rex::imgui::selectionMuted);
		}

		// Tree Node
		//----------

		if (previousState)
		{
			fillWithColour(rex::imgui::selection);
		}

		//if (rex::directory::num_dirs(directory) == 0)
		//{
		//	flags |= ImGuiTreeNodeFlags_Leaf;
		//}

		bool open = ImGui::TreeNodeEx(name.data(), flags);

		if (isActiveDirectory || isItemClicked)
		{
			ImGui::PopStyleColor();
		}

		// Fixing slight overlap
		rex::imgui::shift_cursor_y(3.0f);

		// Create Menu
		//------------
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4.0f, 4.0f));
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::BeginMenu("New"))
			{
				if (ImGui::MenuItem("Folder"))
				{
					if (rex::directory::create(rex::path::join(directory, "New Folder")))
					{
						// Refresh
						REX_INFO(LogContentBrowserWidget, "Refreshing content browser after creating a new folder");
					}
					else
					{
						REX_ERROR(LogContentBrowserWidget, "Failed to create new folder in {}", directory);
					}
				}

				if (ImGui::MenuItem("Scene"))
				{
					REX_INFO(LogContentBrowserWidget, "Creating a new scene asset");
				}
				if (ImGui::MenuItem("Material"))
				{
					REX_INFO(LogContentBrowserWidget, "Creating a new material asset");
				}
				if (ImGui::MenuItem("Sound Config"))
				{
					REX_INFO(LogContentBrowserWidget, "Creating a new sound config asset");
				}
				ImGui::EndMenu();
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Show in Explorer"))
			{
				// Show directory in explorer
			}

			ImGui::EndPopup();
		}
		ImGui::PopStyleVar(); // ItemSpacing

		// Draw children
		//--------------

		if (open)
		{
			rsl::vector<rsl::string> sub_dirs = rex::directory::list_dirs(directory);
			rsl::sort(sub_dirs.begin(), sub_dirs.end());

			for (auto& child : sub_dirs)
			{
				render_directory_hiearchy(child);
			}
		}

		//UpdateDropArea(directory);

		if (open != previousState && !isActiveDirectory)
		{
			if (!ImGui::IsMouseDragging(ImGuiMouseButton_Left, 0.01f))
			{
				REX_INFO(LogContentBrowserWidget, "Changing current directory to {}", directory);
			}
		}

		if (open)
		{
			ImGui::TreePop();
		}
	}

	void ContentBrowserWidget::render_top_bar(f32 height)
	{
		ImGui::BeginChild("##top_bar", ImVec2(0, height));
		ImGui::BeginHorizontal("##top_bar", ImGui::GetWindowSize());
		{
			const float edgeOffset = 4.0f;

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

				//		const float iconSize = std::min(24.0f, height);
				//		const float iconPadding = 3.0f;
				//		const bool clicked = ImGui::Button(labelId, ImVec2(iconSize, iconSize));
				//		rex::imgui::DrawButtonImage(icon, rex::imgui::textDarker,
				//			rex::imgui::ColourWithMultipliedValue(rex::imgui::textDarker, 1.2f),
				//			rex::imgui::ColourWithMultipliedValue(rex::imgui::textDarker, 0.8f),
				//			rex::imgui::RectExpanded(rex::imgui::GetItemRect(), -iconPadding, -iconPadding));

				//		return clicked;
				//	};

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
				rex::imgui::ScopedColour textColour(ImGuiCol_Text, rex::imgui::textDarker);

				rsl::string_view assetsDirectoryName = rex::path::cwd(); //  m_Project->GetConfig().AssetDirectory;
				ImVec2 textSize = ImGui::CalcTextSize(assetsDirectoryName.data());
				const float textPadding = ImGui::GetStyle().FramePadding.y;
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
	void ContentBrowserWidget::render_bottom_bar(f32 height)
	{
		// Display the full filepath of the current selected item as well as its size and other metadata
	}
	void ContentBrowserWidget::render_items()
	{
		//m_IsAnyItemHovered = false;

		rsl::vector<rsl::string_view> items;
		items.reserve(m_directories_in_current_directory.size() + m_files_in_current_directory.size());
		rsl::copy(m_directories_in_current_directory.cbegin(), m_directories_in_current_directory.cend(), rsl::back_inserter(items));
		rsl::copy(m_files_in_current_directory.cbegin(), m_files_in_current_directory.cend(), rsl::back_inserter(items));

		// TODO(Peter): This method of handling actions isn't great... It's starting to become spaghetti...
		for (rsl::string_view item : items)
		{
			item = rex::path::filename(item);

			ImGui::PushID(rsl::hash<rsl::string_view>{}(item));
			ImGui::BeginGroup();

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

			const float edgeOffset = 4.0f;

			const float textLineHeight = ImGui::GetTextLineHeightWithSpacing() * 2.0f + edgeOffset * 2.0f;
			const float infoPanelHeight = textLineHeight;
			const float thumbnailSize = 128.0f; // float(editorSettings.ContentBrowserThumbnailSize);

			const ImVec2 topLeft = ImGui::GetCursorScreenPos();
			const ImVec2 thumbBottomRight = { topLeft.x + thumbnailSize, topLeft.y + thumbnailSize };
			const ImVec2 infoTopLeft = { topLeft.x,				 topLeft.y + thumbnailSize };
			const ImVec2 bottomRight = { topLeft.x + thumbnailSize, topLeft.y + thumbnailSize + infoPanelHeight };

			const bool isFocused = ImGui::IsWindowFocused();

			const bool isSelected = false; // SelectionManager::IsSelected(SelectionContext::ContentBrowser, m_ID);
			auto drawShadow = [](const ImVec2& topLeft, const ImVec2& bottomRight, bool directory)
			{
				auto* drawList = ImGui::GetWindowDrawList();
				const ImRect itemRect = rex::imgui::rect_offset(ImRect(topLeft, bottomRight), 1.0f, 1.0f);
				drawList->AddRect(itemRect.Min, itemRect.Max, rex::imgui::propertyField, 6.0f, directory ? 0 : ImDrawFlags_RoundCornersBottom, 2.0f);
			};

			auto* drawList = ImGui::GetWindowDrawList();

			// Draw shadow
			drawShadow(topLeft, bottomRight, false);

			// Draw background
			drawList->AddRectFilled(topLeft, thumbBottomRight, rex::imgui::backgroundDark);
			drawList->AddRectFilled(infoTopLeft, bottomRight, rex::imgui::groupHeader, 6.0f, ImDrawFlags_RoundCornersBottom);

			// Render thumbnail here
			//ImGui::Text(item.data());

			rex::imgui::shift_cursor(edgeOffset, edgeOffset);
			ImGui::BeginVertical((rsl::string("InfoPanel") + item).c_str(), ImVec2(thumbnailSize - edgeOffset * 2.0f, infoPanelHeight - edgeOffset));
			{
				// Centre align directory name
				ImGui::BeginHorizontal(item.data(), ImVec2(thumbnailSize - 2.0f, 0.0f));
				ImGui::Spring();
				{
					ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + (thumbnailSize - edgeOffset * 3.0f));
					const float textWidth = std::min(ImGui::CalcTextSize(item.data()).x, thumbnailSize);
					//if (m_IsRenaming)
					//{
					//	ImGui::SetNextItemWidth(thumbnailSize - edgeOffset * 3.0f);
					//	renamingWidget();
					//}
					//else
					//{
						ImGui::SetNextItemWidth(textWidth);
						ImGui::Text(item.data());
					//}
					ImGui::PopTextWrapPos();
				}
				ImGui::Spring();
				ImGui::EndHorizontal();

				ImGui::Spring();
			}
			ImGui::EndVertical();
			rex::imgui::shift_cursor(-edgeOffset, -edgeOffset);

			ImGui::PopStyleVar(); // ItemSpacing
			ImGui::EndGroup();

			// Based on the action of the user, update the UI
			// the following actions are supported
			// Selection cleared
			// item selected
			// item deselected
			// ctrl + selection
			// rename
			// open in explorer
			// hovered
			// duplicated
			// renamed
			// refresh

			ImGui::PopID();
			ImGui::NextColumn();

		// This is a workaround an issue with ImGui: https://github.com/ocornut/imgui/issues/331
		//if (s_OpenDeletePopup)
		//{
		//	ImGui::OpenPopup("Delete");
		//	s_OpenDeletePopup = false;
		//}

		//if (s_OpenNewScriptPopup)
		//{
		//	ImGui::OpenPopup("New Script");
		//	s_OpenNewScriptPopup = false;
		//}
		}

	}

}