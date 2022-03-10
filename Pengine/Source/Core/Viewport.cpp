#include "Viewport.h"

#include "Window.h"
#include "Environment.h"
#include "Logger.h"
#include "ImGuizmo.h"
#include "Editor.h"
#include "Input.h"
#include "Serializer.h"
#include "../Events/OnMainThreadCallback.h"
#include "../EventSystem/EventSystem.h"
#include "../OpenGL/FrameBuffer.h"
#include "../UI/Gui.h"

using namespace Pengine;

Viewport& Viewport::GetInstance()
{
    static Viewport viewport;
    return viewport;
}

void Viewport::Begin()
{
    m_FrameBufferViewport->Bind();
    Window::GetInstance().Clear();
    m_PreviousWindowSize = Window::GetInstance().GetSize();
    glViewport(0, 0, m_Size.x, m_Size.y);
}

void Viewport::End()
{
    glViewport(0, 0, m_PreviousWindowSize.x, m_PreviousWindowSize.y);
    m_FrameBufferViewport->UnBind();
}

void Viewport::SetViewPortPosition(const glm::ivec2 position)
{
	m_Position = position;
}

void Viewport::Initialize()
{
    m_Size = Window::GetInstance().GetSize();
    FrameBuffer::FrameBufferParams params = { m_Size, 1, GL_COLOR_ATTACHMENT0, GL_RGBA,
        GL_UNSIGNED_BYTE, true, true, true };
    m_FrameBufferViewport = new FrameBuffer(params, TextureManager::GetInstance().GetTexParamertersi());

    Logger::Success("Viewport has been initialized!");
}

void Viewport::Update()
{
	if (Input::IsKeyPressed(Keycode::KEY_W))
	{
		m_GizmoOperation = 0;
	}
	else if (Input::IsKeyPressed(Keycode::KEY_R))
	{
		m_GizmoOperation = 1;
	}
	else if (Input::IsKeyPressed(Keycode::KEY_S))
	{
		m_GizmoOperation = 2;
	}
	else if (Input::IsKeyPressed(Keycode::KEY_Q))
	{
		m_GizmoOperation = -1;
	}

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("ViewPort", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    if (m_Size.x != ImGui::GetWindowSize().x || m_Size.y != ImGui::GetWindowSize().y)
    {
        Resize(glm::ivec2(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y));
    }

	m_IsHovered = ImGui::IsWindowHovered();
	m_IsFocused = ImGui::IsWindowFocused();

	SetViewPortPosition(glm::vec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y));

    void* texture = reinterpret_cast<void*>(m_FrameBufferViewport->m_Textures[0]);
    ImGui::Image(texture, ImVec2(m_Size.x, m_Size.y), ImVec2(0, 1), ImVec2(1, 0));

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSETS_BROWSER_ITEM"))
		{
			std::string path((const char*)payload->Data);
			path.resize(payload->DataSize);
			std::string resolution = Utils::GetResolutionFromFilePath(path);
			if (resolution == "yaml")
			{
				auto callback = [=]() {
					Serializer::DeserializeScene(path);
				};
				EventSystem::GetInstance().SendEvent(new OnMainThreadCallback(callback, EventType::ONMAINTHREADPROCESS));
			
			}
			else if (resolution == "prefab")
			{
				auto callback = [=]() {
					Serializer::DeserializePrefab(path);
				};
				EventSystem::GetInstance().SendEvent(new OnMainThreadCallback(callback, EventType::ONMAINTHREADPROCESS));
			}
		}
		ImGui::EndDragDropTarget();
	}

	std::shared_ptr<Camera> camera = Environment::GetInstance().GetMainCamera();

	glm::vec2 cameraPos = camera->m_Transform.GetPosition();
	glm::dvec2 normalizedcursorPosition;
	glfwGetCursorPos(Window::GetInstance().GetWindow(), &normalizedcursorPosition.x, &normalizedcursorPosition.y);
	normalizedcursorPosition -= glm::dvec2(m_Position.x, m_Position.y);
	normalizedcursorPosition /= glm::dvec2(m_Size.x, m_Size.y);
	normalizedcursorPosition *= 2.0;
	normalizedcursorPosition -= 1.0;
	normalizedcursorPosition = glm::clamp(normalizedcursorPosition, glm::dvec2(-1.0, -1.0), glm::dvec2(1.0, 1.0));
	normalizedcursorPosition.y *= -1.0;

	glm::vec2 cursorPosition = normalizedcursorPosition * (double)camera->GetZoom();
	cursorPosition.x *= camera->GetAspect();
	cursorPosition += glm::dvec2(cameraPos.x, -cameraPos.y);

	m_PreviousMousePosition = m_MousePosition;
	m_MousePosition = cursorPosition;

	m_PreviousUIMousePosition = m_UIMousePosition;
	m_UIMousePosition = normalizedcursorPosition;
	m_UIMousePosition *= Gui::GetInstance().m_RelativeHeight * 0.5f;
	m_UIMousePosition.x *= camera->GetAspect();

	m_IsActiveGuizmo = false;
	//Need to fix Rotation doesn't change orientation of gizmos
	Editor& editor = Editor::GetInstance();
	std::vector<GameObject*> gameObjects = editor.GetSelectedGameObjects();

	glm::vec3 averagePosition = { 0.0f, 0.0f, 0.0f };
	glm::vec3 averageScale = { 0.0f, 0.0f, 0.0f };
	glm::vec3 averageRotation = { 0.0f, 0.0f, 0.0f };

	struct TransformParams
	{
		glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_Rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_Scale = { 0.0f, 0.0f, 0.0f };
	};

	std::vector<TransformParams> transformParamsDiff;

	for (GameObject* gameObject : gameObjects)
	{
		averagePosition += gameObject->m_Transform.GetPosition();
		averageRotation += gameObject->m_Transform.GetRotation();
		averageScale += gameObject->m_Transform.GetScale();
	}

	averagePosition /= gameObjects.size();
	averageRotation /= gameObjects.size();
	averageScale /= gameObjects.size();

	transformParamsDiff.resize(gameObjects.size());
	for (size_t i = 0; i < transformParamsDiff.size(); i++)
	{
		transformParamsDiff[i].m_Position = gameObjects[i]->m_Transform.GetPosition() - averagePosition;
		transformParamsDiff[i].m_Rotation = gameObjects[i]->m_Transform.GetRotation() - averageRotation;
		transformParamsDiff[i].m_Scale = gameObjects[i]->m_Transform.GetScale() - averageScale;
	}

	ImGuizmo::SetOrthographic(true);
	ImGuizmo::SetDrawlist();

	float windowWidth = (float)ImGui::GetWindowWidth();
	float windowHeight = (float)ImGui::GetWindowHeight();
	ImGuizmo::SetRect(m_Position.x, m_Position.y, windowWidth, windowHeight);

	glm::mat4 ProjectionMat4 = camera->GetProjectionMat4();
	glm::mat4 ViewMat4 = camera->GetViewMat4();
	glm::mat4 transformMat4 = glm::translate(glm::mat4(1.0f), averagePosition)
		* glm::toMat4(glm::quat(averageRotation))
		* glm::scale(glm::mat4(1.0f), averageScale);
	ImGuizmo::Manipulate(glm::value_ptr(ViewMat4), glm::value_ptr(ProjectionMat4),
		(ImGuizmo::OPERATION)m_GizmoOperation, ImGuizmo::LOCAL, glm::value_ptr(transformMat4));

	if (ImGuizmo::IsUsing())
	{
		m_IsActiveGuizmo = true;
		glm::vec3 position, rotation, scale;
		Utils::DecomposeTransform(transformMat4, position, rotation, scale);
		if (camera->GetType() == Camera::CameraType::ORTHOGRAPHIC)
		{
			position.z = 0.0f;
		}

		for (size_t i = 0; i < gameObjects.size(); i++)
		{
			//glm::vec3 deltaRotation = rotation - gameObjects[i]->m_Transform.GetRotation();
			//rotation = gameObjects[i]->m_Transform.GetRotation() + deltaRotation;

			glm::vec3 newPosition = position + transformParamsDiff[i].m_Position;
			if (editor.m_Snap)
			{
				newPosition = glm::round(newPosition);
			}

			gameObjects[i]->m_Transform.Translate(newPosition);
			gameObjects[i]->m_Transform.Rotate(rotation + transformParamsDiff[i].m_Rotation);
			gameObjects[i]->m_Transform.Scale(scale + transformParamsDiff[i].m_Scale);
		}
	}

    ImGui::PopStyleVar();
    ImGui::End();
}

void Viewport::Resize(const glm::ivec2& size)
{
    m_Size = size;
    m_FrameBufferViewport->Resize(m_Size);

    std::shared_ptr<Camera> camera = Environment::GetInstance().GetMainCamera();
    camera->UpdateProjection(m_Size);
	Gui::GetInstance().RecalculateProjectionMatrix();
}
