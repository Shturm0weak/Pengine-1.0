#include "Editor.h"

#include "Viewport.h"
#include "EntryPoint.h"
#include "Application.h"
#include "Environment.h"
#include "Serializer.h"
#include "Input.h"
#include "TextureAtlasManager.h"
#include "../Events/OnMainThreadCallback.h"
#include "../EventSystem/EventSystem.h"
#include "../Components/BoxCollider2D.h"
#include "../Components/Rigidbody2D.h"
#include "../Components/Animator2D.h"
#include "Box2D/include/box2d/b2_fixture.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <filesystem>

using namespace Pengine;

void Editor::Debug()
{
	if (ImGui::Begin("Stats"))
	{
		ImGui::Text("Drawcalls: %zu", m_Stats.m_DrawCalls);
		ImGui::Text("Vertices: %zu", m_Stats.m_Vertices);
		ImGui::Text("Indices: %zu", m_Stats.m_Indices);
		ImGui::Text("Render time: %f", (float)m_Stats.m_RenderTime);
		ImGui::Text("FPS: %.0f", Time::GetFps());
		ImGui::Text("Selected objs: %zu", m_SelectedGameObjects.size());
		ImGui::Text("GameObjects: %zu", m_CurrentScene->m_GameObjects.size());
		ImGui::Text("Renderers 2D: %zu", m_CurrentScene->m_Renderers2D.size());
		ImGui::Text("UUIDs: %zu", UUID::s_UUIDs.size());

		size_t amount = 0;
		for (size_t i = 0; i < m_CurrentScene->m_Renderer2DLayers.size(); i++)
		{
			amount += m_CurrentScene->m_Renderer2DLayers[i].size();
		}
		ImGui::Text("Renderers layer 2D: %zu", amount);
		ImGui::End();
	}
}

void Editor::DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue,
	const glm::vec2& limits, float speed, float columnWidth)
{
	ImGuiIO& io = ImGui::GetIO();

	ImGui::PushID(label.c_str());

	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, columnWidth);
	ImGui::Text(label.c_str());
	ImGui::NextColumn();

	ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

	const float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
	const ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	if (ImGui::Button("X", buttonSize))
	{
		values.x = resetValue;
	}
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::DragFloat("##X", &values.x, speed, limits.x, limits.y, "%.2f");
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
	if (ImGui::Button("Y", buttonSize))
	{
		values.y = resetValue;
	}
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::DragFloat("##Y", &values.y, speed, limits.x, limits.y, "%.2f");
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
	if (ImGui::Button("Z", buttonSize))
	{
		values.z = resetValue;
	}
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::DragFloat("##Z", &values.z, speed, limits.x, limits.y, "%.2f");
	ImGui::PopItemWidth();

	ImGui::PopStyleVar();

	ImGui::Columns(1);

	ImGui::PopID();
}

void Editor::DrawNode(GameObject* gameObject, ImGuiTreeNodeFlags flags)
{
	flags |= gameObject->GetChilds().size() == 0 ? ImGuiTreeNodeFlags_Leaf : 0;
	ImGui::Indent();
	const bool opened = ImGui::TreeNodeEx((void*)gameObject, flags, gameObject->GetName().c_str());
	
	if (ImGui::BeginDragDropSource())
	{
		ImGui::SetDragDropPayload("GAMEOBJECT", (const void*)&gameObject->m_UUID, sizeof(size_t));
		ImGui::EndDragDropSource();
	}

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GAMEOBJECT"))
		{
			auto callback = [=]() {
				for (auto child : m_SelectedGameObjects)
				{
					gameObject->AddChild(child);
				}
			};
			EventSystem::GetInstance().SendEvent(new OnMainThreadCallback(callback, EventType::ONMAINTHREADPROCESS));
		}
		ImGui::EndDragDropTarget();
	}

	if (ImGui::IsItemClicked())
	{
		if (!Input::IsKeyDown(Keycode::KEY_LEFT_CONTROL))
		{
			m_SelectedGameObjects.clear();
		}
		AddSelectedGameObject(gameObject);
	}

	if (opened)
	{
		ImGui::TreePop();
		DrawChilds(gameObject);
	}
	ImGui::Unindent();
}

void Editor::DrawChilds(GameObject* gameObject)
{
	const size_t childsSize = gameObject->GetChilds().size();
	for (uint32_t i = 0; i < childsSize; i++)
	{
		GameObject* child = static_cast<GameObject*>(gameObject->GetChilds()[i]);
		ImGuiTreeNodeFlags flags = Utils::IsThere<GameObject>(m_SelectedGameObjects, child) 
			? ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_Selected : 0;
		DrawNode(child, flags);
	}
}

void Editor::DrawScene()
{
	ImGui::Indent();
	const bool opened = ImGui::TreeNodeEx((void*)&m_CurrentScene, 0, m_CurrentScene->GetTitle().c_str());
	if (opened)
	{
		ImGui::TreePop();

		const std::vector<GameObject*>& gameObjects = m_CurrentScene->GetGameObjects();
		for (auto& objectIter : gameObjects)
		{
			GameObject* gameObject = objectIter;
			if (gameObject->GetOwner() != nullptr) continue;
			ImGuiTreeNodeFlags flags = Utils::IsThere<GameObject>(m_SelectedGameObjects, gameObject)
				? ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_Selected : 0;
			DrawNode(gameObject, flags);
		}
	}
	ImGui::Unindent();
}

void Editor::AssetBrowser()
{
	if (ImGui::Begin("Asset browser"))
	{
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GAMEOBJECT"))
			{
				GameObject* gameObject = m_CurrentScene->FindGameObject(*(size_t*)payload->Data);
				if (gameObject)
				{
					Serializer::SerializePrefab(m_CurrentDirectory.string(), *gameObject);
				}
			}
			ImGui::EndDragDropTarget();
		}

		if (m_CurrentDirectory != std::filesystem::path(m_RootDirectory))
		{
			if (ImGui::Button("<-"))
			{
				m_CurrentDirectory = m_CurrentDirectory.parent_path();
			}
		}

		const float padding = 16.0f;
		const float thumbnailSize = 128.0f;
		const float cellSize = padding + thumbnailSize;
		const float panelWidth = ImGui::GetContentRegionAvailWidth();

		const int columns = (int)(panelWidth / cellSize);
		ImGui::Columns(columns, 0, false);

		const bool leftMouseButtonDoubleClicked = ImGui::IsMouseDoubleClicked(GLFW_MOUSE_BUTTON_1);

		const ImTextureID folderIconId = (ImTextureID)TextureManager::GetInstance().Get("FolderIcon")->GetRendererID();
		const ImTextureID fileIconId = (ImTextureID)TextureManager::GetInstance().Get("FileIcon")->GetRendererID();

		for (auto& directoryIter : std::filesystem::directory_iterator(m_CurrentDirectory))
		{
			const std::string path = directoryIter.path().string();
			if (Utils::Contains(path, ".cpp") || Utils::Contains(path, ".h"))
			{
				continue;
			}

			const std::string filename = Utils::GetNameFromFilePath(path, -1);
			const ImTextureID currentIcon = directoryIter.is_directory() ? folderIconId : fileIconId;
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
			ImGui::PushID(filename.c_str());
			ImGui::ImageButton(currentIcon, { thumbnailSize , thumbnailSize }, ImVec2(0, 1), ImVec2(1, 0));
			ImGui::PopID();
			if (ImGui::BeginDragDropSource())
			{
				const char* assetPath = path.c_str();
				size_t size = strlen(assetPath);
				ImGui::SetDragDropPayload("ASSETS_BROWSER_ITEM", assetPath, size);
				ImGui::EndDragDropSource();
			}

			ImGui::PopStyleColor();
			if (ImGui::IsItemHovered() && leftMouseButtonDoubleClicked)
			{
				if (directoryIter.is_directory())
				{
					m_CurrentDirectory /= directoryIter.path().filename();
				}
			}
			ImGui::TextWrapped("%s", filename.c_str());
			
			ImGui::NextColumn();
		}
		ImGui::Columns(1);
		ImGui::End();
	}
}

void Editor::Hierarchy()
{
	if (ImGui::Begin("Hierarchy"))
	{
		if (ImGui::CollapsingHeader("Game Objects"))
		{
			CreatePopUpMenu();
			DrawScene();
		}
		ImGui::End();
	}
}

#include "Visualizer.h";

void Editor::Properties()
{
	if (ImGui::Begin("Properties"))
	{
		for(GameObject* gameObject : m_SelectedGameObjects)
		{
			ComponentsMenu(gameObject);
			ImGui::Text("ID: %zu", gameObject->GetUUID());
			char name[32];
			strcpy(name, gameObject->GetName().c_str());
			if (ImGui::InputText("Name", name, sizeof(name)))
			{
				gameObject->SetName(name);
			}
			ImGui::Text("Owner: %s", gameObject->HasOwner() ? gameObject->GetOwner()->GetName().c_str() : "Null");
			ImGui::Checkbox("Enable", &gameObject->m_IsEnabled);
			ImGui::Checkbox("Serialize", &gameObject->m_IsSerializable);
			ImGui::Text(gameObject->m_IsInitialized ? "Yes" : "No");
			TransformComponent(gameObject->m_Transform);
			Renderer2DComponent(gameObject);
			Rigidbody2DComponent(gameObject);
			BoxCollider2DComponent(gameObject);
			Animator2DComponent(gameObject);
			ImGui::NewLine();

			glm::mat4 transform = gameObject->m_Transform.GetTransform();
			glm::vec4 a = transform * glm::vec4(-0.5f, -0.5f, 0.0f, 1.0f);
			glm::vec4 b = transform * glm::vec4(0.5f, -0.5f, 0.0f, 1.0f);
			glm::vec4 c = transform * glm::vec4(0.5f, 0.5f, 0.0f, 1.0f);
			glm::vec4 d = transform * glm::vec4(-0.5f, 0.5f, 0.0f, 1.0f);

			Visualizer::DrawLine(a, b, glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
			Visualizer::DrawLine(b, c, glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
			Visualizer::DrawLine(c, d, glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
			Visualizer::DrawLine(d, a, glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
		}
		ImGui::End();
	}
}

void Editor::Navigation()
{
	if (ImGui::BeginMenuBar())
	{
		FilePopUpMenu();
		ViewPopUpMenu();
		ImGui::EndMenuBar();
	}
}

void Editor::Environment()
{
	if (ImGui::Begin("Environment"))
	{
		if (ImGui::CollapsingHeader("Main camera"))
		{
			std::shared_ptr<Camera> camera = Environment::GetInstance().GetMainCamera();
			ImGui::DragFloat("Speed", &camera->m_Speed, 1.0f, 0.0f, 100.0f);
			ImGui::DragFloat("Zoom sensetivity", &camera->m_ZoomSensetivity, 0.1f, 0.1f, 5.0f);
			camera->m_Fov = glm::radians(camera->m_Fov);
			TransformComponent(camera->m_Transform);
		}
		ImGui::End();
	}
}

void Editor::ToolBar()
{
	if (ImGui::Begin("##toolBar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
	{
		const float size = ImGui::GetWindowHeight() - 4.0f;
		ImTextureID icon = EntryPoint::GetApplication().GetState() == Application::ApplicationState::Edit ?
			(ImTextureID)TextureManager::GetInstance().Get("PlayButton")->GetRendererID()
			: (ImTextureID)TextureManager::GetInstance().Get("StopButton")->GetRendererID();
		if (ImGui::ImageButton(icon, { size, size }))
		{
			if (EntryPoint::GetApplication().GetState() == Application::ApplicationState::Play)
			{
				OnApplicationStop();
			}
			else if (EntryPoint::GetApplication().GetState() == Application::ApplicationState::Edit)
			{
				OnApplicationPlay();
			}
		}
		ImGui::End();
	}
}

void Editor::TransformComponent(Transform& transform)
{
	if (ImGui::Button("O"))
	{
	}
	ImGui::SameLine();
	if (ImGui::CollapsingHeader("Transform"))
	{
		glm::vec3 position = transform.GetPosition();
		glm::vec3 rotation = glm::degrees(transform.GetRotation());
		glm::vec3 scale = transform.GetScale();
		DrawVec3Control("Translation", position);
		DrawVec3Control("Rotation", rotation, 0.0f, {-360.0f, 360.0f }, 1.0f);
		DrawVec3Control("Scale", scale, 1.0f, { 0.0f, 25.0f });
		transform.Scale(scale);
		transform.Translate(position);
		transform.Rotate(glm::radians(rotation));
	}
}

void Editor::Renderer2DComponent(GameObject* gameObject)
{
	Renderer2D* r2d = gameObject->m_ComponentManager.GetComponent<Renderer2D>();
	if (r2d)
	{
		ImGui::PushID(r2d->GetUUID());
		if (ImGui::Button("X"))
		{
			gameObject->m_ComponentManager.RemoveComponent(r2d);
		}
		ImGui::PopID();
		ImGui::SameLine();
		if (ImGui::CollapsingHeader("Renderer2D"))
		{
			int layer = r2d->GetLayer();
			if (ImGui::SliderInt("Layer", &layer, 0, 9))
			{
				r2d->SetLayer(layer);
			}
			
			ImGui::ColorEdit4("Color", &r2d->m_Color[0]);
			ImGui::Image((ImTextureID)r2d->GetTexture()->GetRendererID(), { 128.0f, 128.0f }, ImVec2(0, 1), ImVec2(1, 0));
		
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSETS_BROWSER_ITEM"))
				{
					std::string path((const char*)payload->Data);
					path.resize(payload->DataSize);
					r2d->SetTexture(TextureManager::GetInstance().Get(Utils::GetNameFromFilePath(path)));
				}
				ImGui::EndDragDropTarget();
			}
			
			if (ImGui::Button("Original uv"))
			{
				r2d->OriginalUV();
			}
		}
	}
}

void Editor::BoxCollider2DComponent(GameObject* gameObject)
{
	BoxCollider2D* bc2d = gameObject->m_ComponentManager.GetComponent<BoxCollider2D>();
	if (bc2d)
	{
		ImGui::PushID(bc2d->GetUUID());
		if (ImGui::Button("X"))
		{
			gameObject->m_ComponentManager.RemoveComponent(bc2d);
		}
		ImGui::PopID();
		ImGui::SameLine();
		if (ImGui::CollapsingHeader("BoxCollider2D"))
		{
			ImGui::DragFloat("Density", &bc2d->m_Density, 0.1f, 0.0f, 5.0f);
			ImGui::DragFloat("Friction", &bc2d->m_Friction, 0.1f, 0.0f, 5.0f);
			ImGui::DragFloat("Restitution", &bc2d->m_Restitution, 0.1f, 0.0f, 5.0f);
			ImGui::DragFloat("Restitution threshold", &bc2d->m_RestitutionThreshold, 0.1f, 0.0f, 5.0f);
		}
	}
}

void Editor::Rigidbody2DComponent(GameObject* gameObject)
{
	Rigidbody2D* rb2d = gameObject->m_ComponentManager.GetComponent<Rigidbody2D>();
	if (rb2d)
	{
		ImGui::PushID(rb2d->GetUUID());
		if (ImGui::Button("X"))
		{
			gameObject->m_ComponentManager.RemoveComponent(rb2d);
		}
		ImGui::PopID();
		ImGui::SameLine();
		if (ImGui::CollapsingHeader("Rigidbody2D"))
		{
			ImGui::Checkbox("Fixed rotation", &rb2d->m_FixedRotation);
			bool type = rb2d->m_BodyType == Rigidbody2D::BodyType::Static ? true : false;
			ImGui::Checkbox("Type Static", &type);
			rb2d->m_BodyType = type ? Rigidbody2D::BodyType::Static : Rigidbody2D::BodyType::Dynamic;
		}
	}
}

void Editor::Animator2DComponent(GameObject* gameObject)
{
	Animator2D* anim2d = gameObject->m_ComponentManager.GetComponent<Animator2D>();
	if (anim2d)
	{
		ImGui::PushID(anim2d->GetUUID());
		if (ImGui::Button("X"))
		{
			gameObject->m_ComponentManager.RemoveComponent(anim2d);
		}
		ImGui::PopID();
		ImGui::SameLine();
		if (ImGui::CollapsingHeader("Animator 2D"))
		{
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSETS_BROWSER_ITEM"))
				{
					std::string path((const char*)payload->Data);
					path.resize(payload->DataSize);

					if (Animation2DManager::Animation2D* animation = 
						Animation2DManager::GetInstance().Load(path))
					{
						anim2d->AddAnimation(animation);
					}
				}
				ImGui::EndDragDropTarget();
			}

			if (ImGui::ListBox("Animations", &m_Animation2DComponent.m_Selected, anim2d->GetAnimationsNames(), anim2d->m_Animations.size()))
			{
				if (m_Animation2DComponent.m_Selected >= 0)
				{
					anim2d->m_CurrentAnimation = anim2d->m_Animations[m_Animation2DComponent.m_Selected];
				}
			}

			ImGui::InputFloat("Speed", &anim2d->m_Speed, 0.1f, 1.0f, 3);

			const float size = 32.0f;
			ImTextureID icon = anim2d->m_Play ?
				(ImTextureID)TextureManager::GetInstance().Get("StopButton")->GetRendererID()
				: (ImTextureID)TextureManager::GetInstance().Get("PlayButton")->GetRendererID();
			if (ImGui::ImageButton(icon, { size, size }))
			{
				if (anim2d->m_Play)
				{
					anim2d->m_Play = false;
				}
				else
				{
					anim2d->m_Play = true;
				}

			}
		}
	}
}

void Editor::OnApplicationStop()
{
	EventSystem::GetInstance().SendEvent(new IEvent(EventType::ONCLOSE));
	EntryPoint::GetApplication().OnStop();
}

void Editor::OnApplicationPlay()
{
	EventSystem::GetInstance().SendEvent(new IEvent(EventType::ONSTART));
	EntryPoint::GetApplication().OnPlay();
}

void Editor::CreatePopUpMenu()
{
	if (ImGui::BeginPopupContextWindow())
	{
		if (ImGui::MenuItem("Create gameobject"))
		{
			m_CurrentScene->CreateGameObject();
		}
		ImGui::EndPopup();
	}
}

void Editor::ComponentsMenu(GameObject* gameObject)
{
	if (ImGui::BeginPopupContextWindow())
	{
		if (ImGui::MenuItem("Renderer2D"))
		{
			gameObject->m_ComponentManager.AddComponent<Renderer2D>();
		}
		if (ImGui::MenuItem("BoxCollider2D"))
		{
			gameObject->m_ComponentManager.AddComponent<BoxCollider2D>();
		}
		if (ImGui::MenuItem("Rigidbody2D"))
		{
			gameObject->m_ComponentManager.AddComponent<Rigidbody2D>();
		}
		if (ImGui::MenuItem("Animator 2D"))
		{
			gameObject->m_ComponentManager.AddComponent<Animator2D>();
		}
		ImGui::EndPopup();
	}
}

Editor& Editor::GetInstance()
{
	static Editor editor;
	return editor;
}

void Editor::Update(Scene* scene)
{
	m_CurrentScene = scene;

	static bool dockspaceOpen = true;
	static bool optFullscreenPersistant = true;
	bool optFullscreen = optFullscreenPersistant;
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	if (optFullscreen)
	{
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}

	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		windowFlags |= ImGuiWindowFlags_NoBackground;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace", &dockspaceOpen, windowFlags);
	ImGui::PopStyleVar();

	if (optFullscreen)
		ImGui::PopStyleVar(2);

	ImGuiIO& io = ImGui::GetIO();
	ImGuiStyle& style = ImGui::GetStyle();
	const float minWinSizeX = style.WindowMinSize.x;
	style.WindowMinSize.x = 370.0f;
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}

	style.WindowMinSize.x = minWinSizeX;

	Navigation();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });

	Viewport& viewport = Viewport::GetInstance();
	viewport.Update();

	ShortCuts();

	if (!m_Enable)
	{
		ImGui::PopStyleVar();

		ImGui::End();
		return;
	}

	Properties();
	AssetBrowser();
	Hierarchy();
	Debug();
	ToolBar();
	Environment();
	Animation2DMenu();
	TextureAtlasMenu();
	Settings();

	if (Input::IsMousePressed(Keycode::MOUSE_BUTTON_1) && viewport.IsHovered()
		&& !viewport.IsActiveGuizmo())
	{
		std::vector<GameObject*> ignoreMask;
		int maxGameObjects = 1;

		if (!Input::IsKeyDown(Keycode::KEY_LEFT_CONTROL))
		{
			if (m_SelectedGameObjects.size() == 1)
			{
				ignoreMask = m_SelectedGameObjects;
			}

			m_SelectedGameObjects.clear();
			std::vector<GameObject*> selectedGameObjects = m_CurrentScene->SelectGameObject(ignoreMask, maxGameObjects);
			if (!selectedGameObjects.empty())
			{
				for (GameObject* gameObject : selectedGameObjects)
				{
					if (!Utils::Erase<GameObject>(m_SelectedGameObjects, gameObject))
					{
						AddSelectedGameObject(gameObject);
					}
				}
			}
		}
		else
		{
			maxGameObjects = m_SelectedGameObjects.size() + 1;
			std::vector<GameObject*> selectedGameObjects = m_CurrentScene->SelectGameObject(ignoreMask, maxGameObjects);
			if (!selectedGameObjects.empty())
			{
				for (GameObject* gameObject : selectedGameObjects)
				{
					if (Input::IsKeyDown(Keycode::KEY_X))
					{
						Utils::Erase<GameObject>(m_SelectedGameObjects, gameObject);
					}
					else if (selectedGameObjects.size() > 1 && !Utils::IsThere<GameObject>(m_SelectedGameObjects, gameObject))
					{
						AddSelectedGameObject(gameObject);
					}
					else if (selectedGameObjects.size() == 1 && !Utils::Erase<GameObject>(m_SelectedGameObjects, gameObject))
					{
						AddSelectedGameObject(gameObject);
					}
				}
			}
		}
	}

	ImGui::PopStyleVar();

	ImGui::End();
}

void Editor::FilePopUpMenu()
{
	if (ImGui::BeginMenu("File"))
	{
		if (ImGui::MenuItem("New", "Ctrl+N"))
		{
			m_SelectedGameObjects.clear();
			m_CurrentScene->Clear();
		}
		if (ImGui::MenuItem("Open...", "Ctrl+O"))
		{

		}
		if (ImGui::MenuItem("Save", "Ctrl+Shift+S"))
		{
			Serializer::SerializeScene(*m_CurrentScene);
		}
		if (ImGui::MenuItem("Exit"))
		{

		}
		ImGui::EndMenu();
	}
}

void Editor::ViewPopUpMenu()
{
	if (ImGui::BeginMenu("View"))
	{
		if (ImGui::MenuItem("Animation 2D"))
		{
			m_Animation2DMenu.m_Open = true;
		}
		else if (ImGui::MenuItem("Texture atlas"))
		{
			m_TextureAtlasMenu.m_Open = true;
		}
		ImGui::EndMenu();
	}
}

void Editor::Settings()
{
	if (ImGui::Begin("Settings"))
	{
		ImGui::Checkbox("Polygon mode", &m_PolygonMode);
		ImGui::Checkbox("Snap", &m_Snap);
		ImGui::Checkbox("Draw colliders", &m_DrawColliders);
		ImGui::SliderFloat("Line width", &m_LineWidth, 0.1f, 5.0f);
		ImGui::End();
	}
}

void Editor::AddSelectedGameObject(GameObject* gameObject)
{
	if (!Utils::IsThere<GameObject>(m_SelectedGameObjects, gameObject))
	{
		m_SelectedGameObjects.push_back(gameObject);
	}
}

void Editor::ShortCuts()
{
	if (Input::IsKeyDown(Keycode::KEY_LEFT_CONTROL))
	{
		if (Input::IsKeyPressed(Keycode::KEY_D))
		{
			std::vector<GameObject*> newGameObjects;
			for (GameObject* gameObject : m_SelectedGameObjects)
			{
				GameObject* newGameObject = m_CurrentScene->CreateGameObject(gameObject->GetName());
				*newGameObject = *gameObject;
				newGameObjects.push_back(newGameObject);
			}

			if (!m_SelectedGameObjects.empty())
			{
				m_SelectedGameObjects = newGameObjects;
			}
		}
	}

	if (Input::IsKeyPressed(Keycode::KEY_DELETE))
	{
		for (GameObject* gameObject : m_SelectedGameObjects)
		{
			m_CurrentScene->DeleteGameObject(gameObject);
		}
		m_SelectedGameObjects.clear();
	}

	if (Input::IsKeyPressed(Keycode::KEY_F))
	{
		m_Enable = !m_Enable;
	}
}

void Editor::ResetStats()
{
	m_Stats.m_DrawCalls = 0;
	m_Stats.m_Vertices = 0;
	m_Stats.m_Indices = 0;
	m_Stats.m_RenderTime = 0;
}

void Editor::SetDarkThemeColors()
{
	auto& colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };

	// Headers
	colors[ImGuiCol_Header] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
	colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
	colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

	// Buttons
	colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
	colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
	colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

	// Frame BG
	colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
	colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
	colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

	// Tabs
	colors[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
	colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
	colors[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
	colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };

	// Title
	colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
	colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
}

void Editor::Animation2DMenu()
{
	if (m_Animation2DMenu.m_Open && ImGui::Begin("Animation 2D", &m_Animation2DMenu.m_Open, ImGuiWindowFlags_NoCollapse))
	{
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSETS_BROWSER_ITEM"))
			{
				std::string path((const char*)payload->Data);
				path.resize(payload->DataSize);
				Texture* texture = TextureManager::GetInstance().Get(Utils::GetNameFromFilePath(path));
				if (m_Animation2DMenu.m_Animation)
				{
					if (texture)
					{
						m_Animation2DMenu.m_Animation->m_Textures.push_back(texture);
					}
					else if (std::filesystem::is_directory(path))
					{
						std::vector<Texture*> textures = TextureManager::GetInstance().GetTexturesFromFolder(path);
						size_t texturesSize = textures.size();
						for (size_t i = 0; i < texturesSize; i++)
						{
							m_Animation2DMenu.m_Animation->m_Textures.push_back(textures[i]);
						}
					}
				}
				if (Utils::Contains(path, ".anim"))
				{
					m_Animation2DMenu.m_Animation = Animation2DManager::GetInstance().Load(path);
					m_Animation2DMenu.m_Animation->m_Name.copy(m_Animation2DMenu.m_Name, 64);
				}
			}
			ImGui::EndDragDropTarget();
		}

		ImGui::InputText("Name", m_Animation2DMenu.m_Name, 64 * sizeof(char));

		if (!m_Animation2DMenu.m_Animation)
		{
			if (ImGui::Button("Create"))
			{
				m_Animation2DMenu.m_Animation = Animation2DManager::GetInstance().Create(std::string(m_Animation2DMenu.m_Name));
			}
			ImGui::End();
			return;
		}
		else
		{
			ImGui::Text("Filepath: %s", m_Animation2DMenu.m_Animation->m_FilePath.c_str());
		}

		const float padding = 16.0f;
		const float thumbnailSize = 128.0f;
		const float cellSize = padding + thumbnailSize;
		const float panelWidth = ImGui::GetContentRegionAvailWidth();

		const int columns = (int)(panelWidth / cellSize);
		ImGui::Columns(glm::max<int>(columns, 1), 0, false);

		for (size_t i = 0; i < m_Animation2DMenu.m_Animation->m_Textures.size(); i++)
		{
		Texture* texture = m_Animation2DMenu.m_Animation->m_Textures[i];
		if (!texture) continue;

		ImGui::Columns(glm::max<int>(columns, 1), 0, false);
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		ImGui::PushID(texture->GetFilePath().c_str());
		if (ImGui::ImageButton((ImTextureID)texture->GetRendererID(), { thumbnailSize , thumbnailSize }, ImVec2(0, 1), ImVec2(1, 0)))
		{
			m_Animation2DMenu.m_Animation->m_Textures.erase(
				m_Animation2DMenu.m_Animation->m_Textures.begin() + i);
		}
		ImGui::PopID();
		ImGui::PopStyleColor();
		ImGui::NextColumn();
		}
		ImGui::Columns(1);

		if (ImGui::Button("Save"))
		{
			m_Animation2DMenu.m_Animation->m_Name = m_Animation2DMenu.m_Name;
			Animation2DManager::GetInstance().Save(m_Animation2DMenu.m_Animation);
			Serializer::SerializeAnimation2D(m_Animation2DMenu.m_Animation);
		}
		ImGui::SameLine();
		if (ImGui::Button("Clear"))
		{
			m_Animation2DMenu.m_Animation->m_Textures.clear();
		}
		ImGui::SameLine();
		ImGui::Checkbox("Replace current anim file", &Animation2DManager::GetInstance().m_ReplaceCurrentAnimFile);

		ImGui::Text("Loaded animations");
		for (auto& anim : Animation2DManager::GetInstance().m_Animations)
		{
			ImGui::Text("%s", anim.first.c_str());
		}

		ImGui::End();
	}
}

void Editor::TextureAtlasMenu()
{
	if (m_TextureAtlasMenu.m_Open && ImGui::Begin("Texture atlas", &m_TextureAtlasMenu.m_Open, ImGuiWindowFlags_NoCollapse))
	{
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSETS_BROWSER_ITEM"))
			{
				std::string path((const char*)payload->Data);
				path.resize(payload->DataSize);
				if (Utils::Contains(path, ".ta"))
				{
					m_TextureAtlasMenu.m_TextureAtlas = Serializer::DeSerializeTextureAtlas(path);
					m_TextureAtlasMenu.m_Texture = m_TextureAtlasMenu.m_TextureAtlas->GetTexture();
				}
			}
			ImGui::EndDragDropTarget();
		}

		if (m_TextureAtlasMenu.m_Texture)
		{
			ImGui::Image((ImTextureID)m_TextureAtlasMenu.m_Texture->GetRendererID(), { 32.0f , 32.0f }, ImVec2(0, 1), ImVec2(1, 0));

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSETS_BROWSER_ITEM"))
				{
					std::string path((const char*)payload->Data);
					path.resize(payload->DataSize);
					m_TextureAtlasMenu.m_Texture = TextureManager::GetInstance().Create(path);
				}
				ImGui::EndDragDropTarget();
			}
		}

		if (!m_TextureAtlasMenu.m_TextureAtlas)
		{
			ImGui::InputText("Name", m_TextureAtlasMenu.m_Name, 64 * sizeof(char));
			ImGui::InputFloat2("Size", &m_TextureAtlasMenu.m_Size[0], 2);
			if (ImGui::Button("Create"))
			{
				if (m_TextureAtlasMenu.m_Name[0] != '\0' && m_TextureAtlasMenu.m_Size.x != 0.0f && m_TextureAtlasMenu.m_Size.y != 0.0f
					&& m_TextureAtlasMenu.m_Texture->GetName() != "White")
				{
					m_TextureAtlasMenu.m_TextureAtlas = TextureAtlasManager::GetInstance().Create(m_TextureAtlasMenu.m_Name, m_TextureAtlasMenu.m_Size, m_TextureAtlasMenu.m_Texture);
					Serializer::SerializeTextureAtlas(m_TextureAtlasMenu.m_TextureAtlas);
				}
			}
			ImGui::End();
			return;
		}
		else
		{
			ImGui::Text("Name: %s", m_TextureAtlasMenu.m_TextureAtlas->GetName().c_str());
			ImGui::Text("Filepath: %s", m_TextureAtlasMenu.m_TextureAtlas->GetFilePath().c_str());
			ImGui::Text("Size: %.2f %.2f", m_TextureAtlasMenu.m_TextureAtlas->GetSize().x, m_TextureAtlasMenu.m_TextureAtlas->GetSize().y);

			Texture* atlasTexture = m_TextureAtlasMenu.m_TextureAtlas->GetTexture();
			int framePadding = -1;
			int amountOfSpritesX = (float)(atlasTexture->GetSize().x) / (float)(m_TextureAtlasMenu.m_TextureAtlas->GetSize().x);
			int amountOfSpritesY = (float)(atlasTexture->GetSize().y) / (float)(m_TextureAtlasMenu.m_TextureAtlas->GetSize().y);
			for (int i = amountOfSpritesY - 1; i >= 0; i--)
			{
				for (int j = 0; j < amountOfSpritesX; j++)
				{
					std::vector<float> uv = m_TextureAtlasMenu.m_TextureAtlas->GetUV({ j, i });
					ImGui::PushID((i * amountOfSpritesX) + j);
					if (ImGui::ImageButton((void*)(intptr_t)atlasTexture->GetRendererID(), ImVec2(56, 56), ImVec2(uv[0], uv[5]), ImVec2(uv[4], uv[1]), framePadding, ImVec4(1.0f, 1.0f, 1.0f, 0.5f)))
					{
						for (auto& gameObject : m_SelectedGameObjects) {
							Renderer2D* r2d = gameObject->m_ComponentManager.GetComponent<Renderer2D>();
							if (r2d)
							{
								r2d->SetTexture(atlasTexture);
								r2d->SetUV(uv);
							}
						}
					}
					ImGui::PopID();
					ImGui::SameLine();
				}
				ImGui::NewLine();
			}
			ImGui::NewLine();
			if (ImGui::Button("Clear"))
			{
				m_TextureAtlasMenu.m_TextureAtlas = nullptr;
			}
		}

		ImGui::End();
	}
}