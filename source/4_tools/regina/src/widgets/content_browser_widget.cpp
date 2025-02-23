#include "regina/widgets/content_browser_widget.h"

#include "rex_engine/gfx/imgui/imgui_utils.h"

#include "rex_std/algorithm.h"

#include "rex_engine/filesystem/path.h"
#include "rex_engine/filesystem/directory.h"
#include "rex_engine/filesystem/file.h"
#include "rex_engine/filesystem/vfs.h"

namespace regina
{
	constexpr auto accent = IM_COL32(236, 158, 36, 255);
	constexpr auto highlight = IM_COL32(39, 185, 242, 255);
	constexpr auto niceBlue = IM_COL32(83, 232, 254, 255);
	constexpr auto compliment = IM_COL32(78, 151, 166, 255);
	constexpr auto background = IM_COL32(36, 36, 36, 255);
	constexpr auto backgroundDark = IM_COL32(26, 26, 26, 255);
	constexpr auto titlebar = IM_COL32(21, 21, 21, 255);
	constexpr auto titlebarOrange = IM_COL32(186, 66, 30, 255);
	constexpr auto titlebarGreen = IM_COL32(18, 88, 30, 255);
	constexpr auto titlebarRed = IM_COL32(185, 30, 30, 255);
	constexpr auto propertyField = IM_COL32(15, 15, 15, 255);
	constexpr auto text = IM_COL32(192, 192, 192, 255);
	constexpr auto textBrighter = IM_COL32(210, 210, 210, 255);
	constexpr auto textDarker = IM_COL32(128, 128, 128, 255);
	constexpr auto textError = IM_COL32(230, 51, 51, 255);
	constexpr auto muted = IM_COL32(77, 77, 77, 255);
	constexpr auto groupHeader = IM_COL32(47, 47, 47, 255);
	constexpr auto selection = IM_COL32(237, 192, 119, 255);
	constexpr auto selectionMuted = IM_COL32(237, 201, 142, 23);
	constexpr auto backgroundPopup = IM_COL32(50, 50, 50, 255);
	constexpr auto validPrefab = IM_COL32(82, 179, 222, 255);
	constexpr auto invalidPrefab = IM_COL32(222, 43, 43, 255);
	constexpr auto missingMesh = IM_COL32(230, 102, 76, 255);
	constexpr auto meshNotSet = IM_COL32(250, 101, 23, 255);

	DEFINE_LOG_CATEGORY(LogContentBrowserWidget);

	ContentBrowserWidget::ContentBrowserWidget()
	{
		m_current_directory.assign("scripts");
	}

	// We want a list of directories on the left hand side, similar to windows explorer
	// And a list of directories and files on the right hand side
	// The left hand side is 25% of the window, the right hand side is 75% of the window
	bool ContentBrowserWidget::on_update()
	{
		static rsl::vector<rsl::string> dummy_dirs =
		{
			rsl::string("scripts"),
			rsl::string("maps"),
			rsl::string("materials"),
			rsl::string("meshes"),
			rsl::string("textures"),
		};
		static rsl::unordered_map<rsl::string, rsl::vector<rsl::string>> dummy_content
		{
			{ rsl::string("scripts"), rsl::vector<rsl::string> { rsl::string("script_a.cs"), rsl::string("script_b.cs"), rsl::string("script_c.cs") }},
			{ rsl::string("maps"), rsl::vector<rsl::string> { rsl::string("map_a.map"), rsl::string("map_b.map"), rsl::string("map_c.map") }},
			{ rsl::string("meshes"), rsl::vector<rsl::string> { rsl::string("mesh_a.mesh"), rsl::string("mesh_b.mesh"), rsl::string("mesh_c.mesh") }},
			{ rsl::string("textures"), rsl::vector<rsl::string> { rsl::string("texture_a.tex"), rsl::string("texture_b.tex"), rsl::string("texture_c.tex") }},
			{ rsl::string("materials"), rsl::vector<rsl::string> { rsl::string("mat_a.mat"), rsl::string("mat_b.mat"), rsl::string("mat_c.mat") }}
		};

		// Create the left table

		// Create the right table

		if (auto widget = rex::imgui::ScopedWidget("Content Browser"))
		{
			ImGuiTableFlags table_flags = ImGuiTableFlags_Resizable
				| ImGuiTableFlags_SizingFixedFit
				| ImGuiTableFlags_BordersInnerV;

			const rsl::vector<rsl::string>& assets = dummy_content.at(m_current_directory);

			if (ImGui::BeginTable("Assets", 2, table_flags))
			{
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

						for (auto& directory : dummy_dirs)
						{
							render_directory_hiearchy(directory);
						}
					//}

					// Draw side shadow
					//ImRect windowRect = UI::RectExpanded(ImGui::GetCurrentWindow()->Rect(), 0.0f, 10.0f);
					//ImGui::PushClipRect(windowRect.Min, windowRect.Max, false);
					//UI::DrawShadowInner(EditorResources::ShadowTexture, 20, windowRect, 1.0f, windowRect.GetHeight() / 4.0f, false, true, false, false);
					//ImGui::PopClipRect();
				//}
				}
				ImGui::EndChild();

				ImGui::TableSetColumnIndex(1);

				const float topBarHeight = 26.0f;
				const float bottomBarHeight = 32.0f;
				ImGui::BeginChild("##directory_content");
				{
					//for (rsl::string_view asset : assets)
					//{
					//	ImGui::Text(rsl::format("{}", asset).data());
					//}

					ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
					render_bottom_bar(topBarHeight);
					ImGui::PopStyleVar();

					ImGui::Separator();

					ImGui::BeginChild("Scrolling");
					{
						ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
						ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 0.35f));

						ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4.0f, 4.0f));
						if (ImGui::BeginPopupContextWindow(0, 1, false))
						{
							if (ImGui::BeginMenu("New"))
							{
								if (ImGui::MenuItem("Folder"))
								{
									std::filesystem::path filepath = FileSystem::GetUniqueFileName(Project::GetActiveAssetDirectory() / m_CurrentDirectory->FilePath / "New Folder");

									// NOTE(Peter): For some reason creating new directories through code doesn't trigger a file system change?
									bool created = FileSystem::CreateDirectory(filepath);

									if (created)
									{
										Refresh();
										const auto& directoryInfo = GetDirectory(m_CurrentDirectory->FilePath / filepath.filename());
										size_t index = m_CurrentItems.FindItem(directoryInfo->Handle);
										if (index != ContentBrowserItemList::InvalidItem)
										{
											SelectionManager::DeselectAll(SelectionContext::ContentBrowser);
											SelectionManager::Select(SelectionContext::ContentBrowser, directoryInfo->Handle);
											m_CurrentItems[index]->StartRenaming();
										}
									}
								}

								if (ImGui::MenuItem("Scene"))
									CreateAsset<Scene>("New Scene.hscene");

								if (ImGui::BeginMenu("Physics"))
								{
									if (ImGui::MenuItem("Mesh Collider"))
										CreateAsset<MeshColliderAsset>("New Mesh Collider.hmc");

									ImGui::EndMenu();
								}

								if (ImGui::MenuItem("Animation Graph"))
								{
									auto extension = Project::GetEditorAssetManager()->GetDefaultExtensionForAssetType(AssetType::AnimationGraph);
									auto animationGraphAsset = CreateAsset<AnimationGraphAsset>("New Animation Graph" + extension);
									HZ_CORE_VERIFY(AnimationGraphAssetSerializer::TryLoadData("Resources/Animation/EmptyAnimationGraph" + extension, animationGraphAsset));
									AssetImporter::Serialize(animationGraphAsset);
								}

								if (ImGui::MenuItem("Material"))
									CreateAsset<MaterialAsset>("New Material.hmaterial");

								if (ImGui::BeginMenu("Audio"))
								{
									if (ImGui::MenuItem("Sound Config"))
										CreateAsset<SoundConfig>("New Sound Config.hsoundc");

									if (ImGui::MenuItem("SoundGraph Sound"))
										CreateAsset<SoundGraphAsset>("New SoundGraph Sound.sound_graph");

									ImGui::EndMenu();
								}

								if (ImGui::MenuItem("Script"))
									s_OpenNewScriptPopup = true;

								ImGui::EndMenu();
							}

							if (ImGui::MenuItem("Import"))
							{
								std::filesystem::path filepath = FileSystem::OpenFileDialog();
								if (!filepath.empty())
								{
									FileSystem::CopyFile(filepath, Project::GetActiveAssetDirectory() / m_CurrentDirectory->FilePath);
									Refresh();
								}
							}

							if (ImGui::MenuItem("Refresh"))
								Refresh();

							if (ImGui::MenuItem("Copy", "Ctrl+C", nullptr, SelectionManager::GetSelectionCount(SelectionContext::ContentBrowser) > 0))
								m_CopiedAssets.CopyFrom(SelectionManager::GetSelections(SelectionContext::ContentBrowser));

							if (ImGui::MenuItem("Paste", "Ctrl+V", nullptr, m_CopiedAssets.SelectionCount() > 0))
								PasteCopiedAssets();

							if (ImGui::MenuItem("Duplicate", "Ctrl+D", nullptr, SelectionManager::GetSelectionCount(SelectionContext::ContentBrowser) > 0))
							{
								m_CopiedAssets.CopyFrom(SelectionManager::GetSelections(SelectionContext::ContentBrowser));
								PasteCopiedAssets();
							}

							ImGui::Separator();

							if (ImGui::MenuItem("Show in Explorer"))
								FileSystem::OpenDirectoryInExplorer(Project::GetActiveAssetDirectory() / m_CurrentDirectory->FilePath);

							ImGui::EndPopup();
						}
						ImGui::PopStyleVar(); // ItemSpacing

						const float paddingForOutline = 2.0f;
						const float scrollBarrOffset = 20.0f + ImGui::GetStyle().ScrollbarSize;
						float panelWidth = ImGui::GetContentRegionAvail().x - scrollBarrOffset;
						float cellSize = EditorApplicationSettings::Get().ContentBrowserThumbnailSize + s_Padding + paddingForOutline;
						int columnCount = (int)(panelWidth / cellSize);
						if (columnCount < 1) columnCount = 1;

						{
							const float rowSpacing = 12.0f;
							UI::ScopedStyle spacing(ImGuiStyleVar_ItemSpacing, ImVec2(paddingForOutline, rowSpacing));
							ImGui::Columns(columnCount, 0, false);

							UI::ScopedStyle border(ImGuiStyleVar_FrameBorderSize, 0.0f);
							UI::ScopedStyle padding(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
							RenderItems();
						}

						if (ImGui::IsWindowFocused() && !ImGui::IsMouseDragging(ImGuiMouseButton_Left))
						{
							UpdateInput();
						}

						ImGui::PopStyleColor(2);

						RenderDeleteDialogue();
						RenderNewScriptDialogue();
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
		rsl::string name(directory);
		rsl::string id = name + "_TreeNode";
		bool previousState = ImGui::TreeNodeBehaviorIsOpen(ImGui::GetID(id.c_str()));

		// ImGui item height hack
		auto* window = ImGui::GetCurrentWindow();
		window->DC.CurrLineSize.y = 20.0f;
		window->DC.CurrLineTextBaseOffset = 3.0f;
		//---------------------------------------------

		const ImRect itemRect = { window->WorkRect.Min.x, window->DC.CursorPos.y,
									window->WorkRect.Max.x, window->DC.CursorPos.y + window->DC.CurrLineSize.y };

		const bool isItemClicked = [&itemRect, &id]
			{
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
				//if (directory->Handle == m_CurrentDirectory->Handle)
				//	return true;

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
		const bool isActiveDirectory = false; // directory->Handle == m_CurrentDirectory->Handle;

		ImGuiTreeNodeFlags flags = (isActiveDirectory ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_SpanFullWidth;

		// Fill background
		//----------------
		if (isActiveDirectory || isItemClicked)
		{
			if (isWindowFocused)
				fillWithColour(selection);
			else
			{
				const ImColor col = rex::imgui::color_with_multiplied_value(selection, 0.8f);
				fillWithColour(rex::imgui::color_with_multiplied_value(col, 0.7f));
			}

			ImGui::PushStyleColor(ImGuiCol_Text, backgroundDark);
		}
		else if (isAnyDescendantSelected)
		{
			fillWithColour(selectionMuted);
		}

		// Tree Node
		//----------

		bool open = ImGui::TreeNode(name.c_str());
		//bool open = UI::TreeNode(id, name, flags, EditorResources::FolderIcon);

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
					//bool created = FileSystem::CreateDirectory(Project::GetActiveAssetDirectory() / directory->FilePath / "New Folder");
					//if (created)
					//	Refresh();
				}

				if (ImGui::MenuItem("Scene"))
				{
					REX_INFO(LogContentBrowserWidget, "Creating a new scene asset");
					//CreateAssetInDirectory<Scene>("New Scene.hscene", directory);
				}
				if (ImGui::MenuItem("Material"))
				{
					REX_INFO(LogContentBrowserWidget, "Creating a new material asset");
					//CreateAssetInDirectory<MaterialAsset>("New Material.hmaterial", directory);
				}
				if (ImGui::MenuItem("Sound Config"))
				{
					REX_INFO(LogContentBrowserWidget, "Creating a new sound config asset");
					//CreateAssetInDirectory<SoundConfig>("New Sound Config.hsoundc", directory);
				}
				ImGui::EndMenu();
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Show in Explorer"))
			{
				// FileSystem::OpenDirectoryInExplorer(Project::GetActiveAssetDirectory() / directory->FilePath);
			}

			ImGui::EndPopup();
		}
		ImGui::PopStyleVar(); // ItemSpacing

		// Draw children
		//--------------

		if (open)
		{
			rsl::vector<rsl::string> sub_dirs = rex::directory::list_dirs(directory);
			//directories.reserve(m_BaseDirectory->SubDirectories.size());
			//for (auto& [handle, directory] : directory->SubDirectories)
			//	directories.emplace_back(directory);

			//rsl::sort(sub_dirs.begin(), sub_dirs.end(), [](const auto& a, const auto& b)
			//	{
			//		return a->FilePath.stem().string() < b->FilePath.stem().string();
			//	});

			for (auto& child : sub_dirs)
				render_directory_hiearchy(child);
		}

		//UpdateDropArea(directory);

		//if (open != previousState && !isActiveDirectory)
		//{
		//	if (!ImGui::IsMouseDragging(ImGuiMouseButton_Left, 0.01f))
		//		ChangeDirectory(directory);
		//}

		if (open)
			ImGui::TreePop();
	}

	void ContentBrowserWidget::render_top_bar(f32 height)
	{
		ImGui::BeginChild("##top_bar", ImVec2(0, height));
		ImGui::BeginHorizontal("##top_bar", ImGui::GetWindowSize());
		{
			const float edgeOffset = 4.0f;

			// Navigation buttons
			{
				UI::ScopedStyle spacing(ImGuiStyleVar_ItemSpacing, ImVec2(2.0f, 0.0f));

				auto contenBrowserButton = [height](const char* labelId, const Ref<Texture2D>& icon)
					{
						const ImU32 buttonCol = Colors::Theme::backgroundDark;
						const ImU32 buttonColP = UI::ColourWithMultipliedValue(Colors::Theme::backgroundDark, 0.8f);
						UI::ScopedColourStack buttonColours(ImGuiCol_Button, buttonCol,
							ImGuiCol_ButtonHovered, buttonCol,
							ImGuiCol_ButtonActive, buttonColP);

						const float iconSize = std::min(24.0f, height);
						const float iconPadding = 3.0f;
						const bool clicked = ImGui::Button(labelId, ImVec2(iconSize, iconSize));
						UI::DrawButtonImage(icon, Colors::Theme::textDarker,
							UI::ColourWithMultipliedValue(Colors::Theme::textDarker, 1.2f),
							UI::ColourWithMultipliedValue(Colors::Theme::textDarker, 0.8f),
							UI::RectExpanded(UI::GetItemRect(), -iconPadding, -iconPadding));

						return clicked;
					};

				if (contenBrowserButton("##back", EditorResources::BackIcon))
				{
					OnBrowseBack();
				}
				UI::SetTooltip("Previous directory");

				ImGui::Spring(-1.0f, edgeOffset);

				if (contenBrowserButton("##forward", EditorResources::ForwardIcon))
				{
					OnBrowseForward();
				}
				UI::SetTooltip("Next directory");

				ImGui::Spring(-1.0f, edgeOffset * 2.0f);

				if (contenBrowserButton("##refresh", EditorResources::RefreshIcon))
				{
					Refresh();
				}
				UI::SetTooltip("Refresh");
				if (contenBrowserButton("##clearThumbnailCache", EditorResources::ClearIcon))
				{
					m_ThumbnailCache->Clear();
				}
				UI::SetTooltip("Clear thumbnail cache");

				ImGui::Spring(-1.0f, edgeOffset * 2.0f);
			}

			// Search
			{
				UI::ShiftCursorY(2.0f);
				ImGui::SetNextItemWidth(200);

				if (s_ActivateSearchWidget)
				{
					ImGui::SetKeyboardFocusHere();
					s_ActivateSearchWidget = false;
				}

				if (UI::Widgets::SearchWidget<MAX_INPUT_BUFFER_LENGTH>(m_SearchBuffer))
				{
					if (strlen(m_SearchBuffer) == 0)
					{
						ChangeDirectory(m_CurrentDirectory);
					}
					else
					{
						m_CurrentItems = Search(m_SearchBuffer, m_CurrentDirectory);
						SortItemList();
					}
				}
				UI::ShiftCursorY(-2.0f);
			}

			if (m_UpdateNavigationPath)
			{
				m_BreadCrumbData.clear();

				Ref<DirectoryInfo> current = m_CurrentDirectory;
				while (current && current->Parent != nullptr)
				{
					m_BreadCrumbData.push_back(current);
					current = current->Parent;
				}

				std::reverse(m_BreadCrumbData.begin(), m_BreadCrumbData.end());
				m_UpdateNavigationPath = false;
			}

			// Breadcrumbs
			{
				UI::ScopedFont boldFont(ImGui::GetIO().Fonts->Fonts[0]);
				UI::ScopedColour textColour(ImGuiCol_Text, Colors::Theme::textDarker);

				const std::string& assetsDirectoryName = m_Project->GetConfig().AssetDirectory;
				ImVec2 textSize = ImGui::CalcTextSize(assetsDirectoryName.c_str());
				const float textPadding = ImGui::GetStyle().FramePadding.y;
				if (ImGui::Selectable(assetsDirectoryName.c_str(), false, 0, ImVec2(textSize.x, textSize.y + textPadding)))
				{
					SelectionManager::DeselectAll(SelectionContext::ContentBrowser);
					ChangeDirectory(m_BaseDirectory);
				}
				UpdateDropArea(m_BaseDirectory);

				for (auto& directory : m_BreadCrumbData)
				{
					ImGui::Text("/");

					std::string directoryName = directory->FilePath.filename().string();
					ImVec2 textSize = ImGui::CalcTextSize(directoryName.c_str());
					if (ImGui::Selectable(directoryName.c_str(), false, 0, ImVec2(textSize.x, textSize.y + textPadding)))
					{
						SelectionManager::DeselectAll(SelectionContext::ContentBrowser);
						ChangeDirectory(directory);
					}

					UpdateDropArea(directory);
				}
			}

			// Settings button
			ImGui::Spring();
			if (UI::Widgets::OptionsButton())
			{
				ImGui::OpenPopup("ContentBrowserSettings");
			}
			UI::SetTooltip("Content Browser settings");


			if (UI::BeginPopup("ContentBrowserSettings"))
			{
				auto& editorSettings = EditorApplicationSettings::Get();

				bool saveSettings = ImGui::MenuItem("Show Asset Types", nullptr, &editorSettings.ContentBrowserShowAssetTypes);
				saveSettings |= ImGui::SliderInt("##thumbnail_size", &editorSettings.ContentBrowserThumbnailSize, 96, 512);
				UI::SetTooltip("Thumnail Size");

				if (saveSettings)
					EditorApplicationSettingsSerializer::SaveSettings();

				UI::EndPopup();
			}

		}
		ImGui::EndHorizontal();
		ImGui::EndChild();
	}
	void ContentBrowserWidget::render_bottom_bar(f32 height)
	{

	}

}

