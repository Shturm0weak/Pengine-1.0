#include "Editor.h"

#include "Window.h"
#include "Viewport.h"
#include "Renderer.h"
#include "EntryPoint.h"
#include "Application.h"
#include "Environment.h"
#include "Serializer.h"
#include "Input.h"
#include "Visualizer.h";
#include "TextureAtlasManager.h"
#include "Timer.h"
#include "ReflectionSystem.h"
#include "Instancing.h"
#include "MaterialManager.h"
#include "Renderer.h"
#include "../UI/Gui.h"
#include "../Events/OnMainThreadCallback.h"
#include "../EventSystem/EventSystem.h"
#include "../Components/BoxCollider2D.h"
#include "../Components/Rigidbody2D.h"
#include "../Components/Animator2D.h"
#include "../Components/ParticleEmitter.h"
#include "../Components/Script.h"
#include "../Components/Spline.h"
#include "../Audio/SoundManager.h"
#include "../Lua/LuaStateManager.h"
#include "../OpenGL/Material.h"
#include "Box2D/include/box2d/b2_fixture.h"

#include <filesystem>
#include <fstream>

using namespace Pengine;

void Editor::Debug()
{
	if (ImGui::Begin("Stats"))
	{
		ImGui::Text("FPS: %.0f", Time::GetFps());
		ImGui::Text("Delta time: %.3f", Time::GetDeltaTime() * 1000.0f);

		if (ImGui::CollapsingHeader("Render time"))
		{
			ImGui::Text("GPU: %.3fms", (float)m_Stats.m_RenderTime);
			ImGui::Text("Bloom: %.3fms", (float)m_Stats.m_RenderBloomTime);
			ImGui::Text("SSAO: %.3fms", (float)m_Stats.m_RenderSSAOTime);
			ImGui::Text("Scene: %.3fms", (float)m_Stats.m_RenderSceneTime);
			ImGui::Text("Compose: %.3fms", (float)m_Stats.m_RenderComposeTime);
			ImGui::Text("UI: %.3fms", (float)m_Stats.m_RenderUITime);
			ImGui::Text("Lines: %.3fms", (float)m_Stats.m_RenderLinesTime);
			ImGui::Text("Editor: %.3fms", (float)m_Stats.m_RenderEditor);
			ImGui::Text("GBuffer: %.3fms", (float)m_Stats.m_RenderGBufferTime);
			ImGui::Text("Shadows: %.3fms", (float)m_Stats.m_RenderShadowsTime);
			ImGui::Text("New Frame: %.3fms", (float)m_Stats.m_NewFrameTime);
			ImGui::Text("End Frame: %.3fms", (float)m_Stats.m_EndFrameTime);
		}

		if (ImGui::CollapsingHeader("Render Stats"))
		{
			ImGui::Text("Drawcalls: %zu", m_Stats.m_DrawCalls);
			ImGui::Text("Triangles: %zu", m_Stats.m_Triangles);
		}

		if (ImGui::CollapsingHeader("Misc"))
		{
			ImGui::Text("Allocations: %zu", m_Stats.s_AllocationsCount);
			ImGui::Text("RTTR classes: %zu", ReflectionSystem::GetInstance().m_ClassesByType.size());
			ImGui::Text("Selected objs: %zu", m_SelectedGameObjects.size());
			ImGui::Text("GameObjects: %zu", m_CurrentScene->m_GameObjects.size());
			ImGui::Text("Events: %zu", EventSystem::GetInstance().m_CurrentEvents.size());
			ImGui::Text("Lua states: %zu", LuaStateManager::GetInstance().m_LuaStates.size());
			ImGui::Text("Raw Lua states: %zu", LuaStateManager::GetInstance().m_LuaStatesRaw.size());
			ImGui::Text("Meshes: %zu", MeshManager::GetInstance().m_Meshes.size());

			size_t amount = 0;
			for (size_t i = 0; i < m_CurrentScene->m_Renderer2DLayers.size(); i++)
			{
				amount += m_CurrentScene->m_Renderer2DLayers[i].size();
			}
			ImGui::Text("Renderers layer 2D: %zu", amount);
		}

		ResetStats();

		ImGui::End();
	}
}

bool Editor::DrawVec2Control(const std::string& label, glm::vec2& values, float resetValue, const glm::vec2& limits, float speed, float columnWidth)
{
	bool changed = false;

	ImGuiIO& io = ImGui::GetIO();

	ImGui::PushID(label.c_str());

	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, columnWidth);
	if(m_DrawVecLabel) ImGui::Text(label.c_str());
	ImGui::NextColumn();

	ImGui::PushMultiItemsWidths(2, ImGui::CalcItemWidth());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 5.0f });

	const float lineHeight = ImGui::GetFont()->FontSize + ImGui::GetStyle().FramePadding.y * 2.0f;
	const ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	if (ImGui::Button("X", buttonSize))
	{
		values.x = resetValue;
		changed = true;
	}
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	if (ImGui::DragFloat("##X", &values.x, speed, limits.x, limits.y, "%.2f"))
	{
		changed = true;
	}
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
	if (ImGui::Button("Y", buttonSize))
	{
		values.y = resetValue;
		changed = true;
	}
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	if(ImGui::DragFloat("##Y", &values.y, speed, limits.x, limits.y, "%.2f"))
	{
		changed = true;
	}
	ImGui::PopItemWidth();

	ImGui::PopStyleVar();

	ImGui::Columns(1);

	ImGui::PopID();

	return changed;
}

bool Editor::DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue,
	const glm::vec2& limits, float speed, float columnWidth)
{
	bool changed = false;

	ImGuiIO& io = ImGui::GetIO();

	ImGui::PushID(label.c_str());

	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, columnWidth);
	if (m_DrawVecLabel) ImGui::Text(label.c_str());
	ImGui::NextColumn();

	ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 5.0f });

	const float lineHeight = ImGui::GetFont()->FontSize + ImGui::GetStyle().FramePadding.y * 2.0f;
	const ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	if (ImGui::Button("X", buttonSize))
	{
		values.x = resetValue;
		changed = true;
	}
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	if(ImGui::DragFloat("##X", &values.x, speed, limits.x, limits.y, "%.2f"))
	{
		changed = true;
	}
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
	if (ImGui::Button("Y", buttonSize))
	{
		values.y = resetValue;
		changed = true;
	}
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	if(ImGui::DragFloat("##Y", &values.y, speed, limits.x, limits.y, "%.2f"))
	{
		changed = true;
	}
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
	if (ImGui::Button("Z", buttonSize))
	{
		values.z = resetValue;
		changed = true;
	}
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	if(ImGui::DragFloat("##Z", &values.z, speed, limits.x, limits.y, "%.2f"))
	{
		changed = true;
	}
	ImGui::PopItemWidth();

	ImGui::PopStyleVar();

	ImGui::Columns(1);

	ImGui::PopID();

	return changed;
}

bool Editor::DrawVec4Control(const std::string& label, glm::vec4& values, float resetValue, const glm::vec2& limits, float speed, float columnWidth)
{
	bool changed = false;

	ImGuiIO& io = ImGui::GetIO();

	ImGui::PushID(label.c_str());

	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, columnWidth);
	if (m_DrawVecLabel) ImGui::Text(label.c_str());
	ImGui::NextColumn();

	ImGui::PushMultiItemsWidths(4, ImGui::CalcItemWidth());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 5.0f });

	const float lineHeight = ImGui::GetFont()->FontSize + ImGui::GetStyle().FramePadding.y * 2.0f;
	const ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	if (ImGui::Button("X", buttonSize))
	{
		values.x = resetValue;
		changed = true;
	}
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	if(ImGui::DragFloat("##X", &values.x, speed, limits.x, limits.y, "%.2f"))
	{
		changed = true;
	}
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
	if (ImGui::Button("Y", buttonSize))
	{
		values.y = resetValue;
		changed = true;
	}
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	if(ImGui::DragFloat("##Y", &values.y, speed, limits.x, limits.y, "%.2f"))
	{
		changed = true;
	}
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
	if (ImGui::Button("Z", buttonSize))
	{
		values.z = resetValue;
		changed = true;
	}
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	if(ImGui::DragFloat("##Z", &values.z, speed, limits.x, limits.y, "%.2f"))
	{
		changed = true;
	}
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.7f, 0.7f, 0.7f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.9f, 0.9f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.7f, 0.7f, 0.7f, 1.0f });
	if (ImGui::Button("W", buttonSize))
	{
		values.w = resetValue;
		changed = true;
	}
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	if(ImGui::DragFloat("##W", &values.w, speed, limits.x, limits.y, "%.2f"))
	{
		changed = true;
	}
	ImGui::PopItemWidth();

	ImGui::PopStyleVar();

	ImGui::Columns(1);

	ImGui::PopID();

	return changed;
}

bool Editor::DrawIVec2Control(const std::string& label, glm::ivec2& values, float resetValue, const glm::vec2& limits, float speed, float columnWidth)
{
	bool changed = false;

	ImGuiIO& io = ImGui::GetIO();

	ImGui::PushID(label.c_str());

	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, columnWidth);
	if (m_DrawVecLabel) ImGui::Text(label.c_str());
	ImGui::NextColumn();

	ImGui::PushMultiItemsWidths(2, ImGui::CalcItemWidth());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 5.0f });

	const float lineHeight = ImGui::GetFont()->FontSize + ImGui::GetStyle().FramePadding.y * 2.0f;
	const ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	if (ImGui::Button("X", buttonSize))
	{
		values.x = resetValue;
		changed = true;
	}
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	if(ImGui::DragInt("##X", &values.x, speed, limits.x, limits.y))
	{
		changed = true;
	}
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
	if (ImGui::Button("Y", buttonSize))
	{
		values.y = resetValue;
		changed = true;
	}
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	if(ImGui::DragInt("##Y", &values.y, speed, limits.x, limits.y))
	{
		changed = true;
	}
	ImGui::PopItemWidth();

	ImGui::PopStyleVar();

	ImGui::Columns(1);

	ImGui::PopID();

	return changed;
}

bool Editor::DrawIVec3Control(const std::string& label, glm::ivec3& values, float resetValue, const glm::vec2& limits, float speed, float columnWidth)
{
	bool changed = false;

	ImGuiIO& io = ImGui::GetIO();

	ImGui::PushID(label.c_str());

	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, columnWidth);
	if (m_DrawVecLabel) ImGui::Text(label.c_str());
	ImGui::NextColumn();

	ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 5.0f });

	const float lineHeight = ImGui::GetFont()->FontSize + ImGui::GetStyle().FramePadding.y * 2.0f;
	const ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	if (ImGui::Button("X", buttonSize))
	{
		values.x = resetValue;
		changed = true;
	}
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	if(ImGui::DragInt("##X", &values.x, speed, limits.x, limits.y))
	{
		changed = true;
	}
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
	if (ImGui::Button("Y", buttonSize))
	{
		values.y = resetValue;
		changed = true;
	}
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	if(ImGui::DragInt("##Y", &values.y, speed, limits.x, limits.y))
	{
		changed = true;
	}
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
	if (ImGui::Button("Z", buttonSize))
	{
		values.z = resetValue;
		changed = true;
	}
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	if(ImGui::DragInt("##Z", &values.z, speed, limits.x, limits.y))
	{
		changed = true;
	}
	ImGui::PopItemWidth();

	ImGui::PopStyleVar();

	ImGui::Columns(1);

	ImGui::PopID();

	return changed;
}

bool Editor::DrawIVec4Control(const std::string& label, glm::ivec4& values, float resetValue, const glm::vec2& limits, float speed, float columnWidth)
{
	bool changed = false;

	ImGuiIO& io = ImGui::GetIO();

	ImGui::PushID(label.c_str());

	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, columnWidth);
	if (m_DrawVecLabel) ImGui::Text(label.c_str());
	ImGui::NextColumn();

	ImGui::PushMultiItemsWidths(4, ImGui::CalcItemWidth());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 5.0f });

	const float lineHeight = ImGui::GetFont()->FontSize + ImGui::GetStyle().FramePadding.y * 2.0f;
	const ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	if (ImGui::Button("X", buttonSize))
	{
		values.x = resetValue;
		changed = true;
	}
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	if(ImGui::DragInt("##X", &values.x, speed, limits.x, limits.y))
	{
		changed = true;
	}
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
	if (ImGui::Button("Y", buttonSize))
	{
		values.y = resetValue;
		changed = true;
	}
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	if(ImGui::DragInt("##Y", &values.y, speed, limits.x, limits.y))
	{
		changed = true;
	}
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
	if (ImGui::Button("Z", buttonSize))
	{
		values.z = resetValue;
		changed = true;
	}
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	if(ImGui::DragInt("##Z", &values.z, speed, limits.x, limits.y))
	{
		changed = true;
	}
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.7f, 0.7f, 0.7f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.9f, 0.9f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.7f, 0.7f, 0.7f, 1.0f });
	if (ImGui::Button("W", buttonSize))
	{
		values.w = resetValue;
		changed = true;
	}
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	if(ImGui::DragInt("##W", &values.w, speed, limits.x, limits.y))
	{
		changed = true;
	}
	ImGui::PopItemWidth();

	ImGui::PopStyleVar();

	ImGui::Columns(1);

	ImGui::PopID();

	return changed;
}

void Editor::DrawNode(GameObject* gameObject, ImGuiTreeNodeFlags flags)
{
	if (!gameObject->m_IsEditorVisible)
	{
		return;
	}
	flags |= gameObject->GetChilds().size() == 0 ? ImGuiTreeNodeFlags_Leaf : 0;

	ImGui::Indent();

	ImGui::PushID(gameObject);
	ImGui::Checkbox("##IsEnabled", &gameObject->m_IsEnabled);
	ImGui::PopID();

	ImGui::SameLine();

	ImGuiStyle* style = &ImGui::GetStyle();

	if (gameObject->IsPrefab())
	{
		style->Colors[ImGuiCol_Text] = ImVec4(0.0f, 0.8f, 1.0f, 1.0f);
	}

	const bool opened = ImGui::TreeNodeEx((void*)gameObject, flags, gameObject->GetName().c_str());
	style->Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

	if (ImGui::BeginDragDropSource())
	{
		ImGui::SetDragDropPayload("GAMEOBJECT", (const void*)gameObject->m_UUID.Get().c_str(), gameObject->m_UUID.Get().size());
		ImGui::EndDragDropSource();
	}

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GAMEOBJECT"))
		{
			std::string uuid((const char*)payload->Data);
			uuid.resize(payload->DataSize);
			auto callback = [gameObject, uuid, scene = m_CurrentScene]()
			{
				GameObject* child = scene->FindGameObjectByUUID(uuid);
				if (child)
				{
					gameObject->AddChild(child);
				}
			};
			EventSystem::GetInstance().SendCallbackOnFrame(callback);
		}
		ImGui::EndDragDropTarget();
	}

	if (ImGui::IsItemHovered() && Input::Mouse::IsMouseReleased(Keycode::MOUSE_BUTTON_1))
	{
		if (!Input::KeyBoard::IsKeyDown(Keycode::KEY_LEFT_CONTROL))
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
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= Utils::IsThere<std::string>(m_SelectedGameObjects, child->GetUUID())
			?  ImGuiTreeNodeFlags_Selected : 0;
		DrawNode(child, flags);
	}
}

void Editor::AssetList()
{
	if (m_IsActiveAssetList && ImGui::Begin("Asset list", &m_IsActiveAssetList, ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse))
	{
		for (auto& texture : TextureManager::GetInstance().m_Textures)
		{
			if (ImGui::Button(texture.first.c_str()))
			{
				m_TextureMenu.m_IsActive = true;
				m_TextureMenu.m_Texture = texture.second;
			}
		}
		ImGui::End();
	}
}

void Editor::DrawScene()
{
	ImGui::Indent();
	
	if (ImGui::TreeNodeEx((void*)&m_CurrentScene, 0, m_CurrentScene->GetTitle().c_str()))
	{
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GAMEOBJECT"))
			{
				std::string uuid((const char*)payload->Data);
				uuid.resize(payload->DataSize);
				auto callback = [=]() 
				{
					GameObject* gameObject = m_CurrentScene->FindGameObjectByUUID(uuid);
					if (gameObject)
					{
						if (GameObject* owner = gameObject->GetOwner())
						{
							owner->RemoveChild(gameObject);
						}
					}
				};
				EventSystem::GetInstance().SendEvent(new OnMainThreadCallback(callback, EventType::ONMAINTHREADPROCESS));
			}
			ImGui::EndDragDropTarget();
		}

		ImGui::TreePop();

		const std::vector<GameObject*>& gameObjects = m_CurrentScene->GetGameObjects();
		for (auto& objectIter : gameObjects)
		{
			GameObject* gameObject = objectIter;
			if (gameObject->GetOwner()) continue;
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
			flags |= Utils::IsThere<std::string>(m_SelectedGameObjects, gameObject->GetUUID())
				? ImGuiTreeNodeFlags_Selected : 0;
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
				std::string uuid((const char*)payload->Data);
				uuid.resize(payload->DataSize);
				GameObject* gameObject = m_CurrentScene->FindGameObjectByUUID(uuid);
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

			ImGui::SameLine();
		}

		ImGui::InputText("Filter", m_FilterBuffer, 64);

		const float padding = 16.0f;
		const float thumbnailSize = 128.0f * m_ThumbnailScale;
		const float cellSize = padding + thumbnailSize;
		const float panelWidth = ImGui::GetContentRegionAvail().x;
		
		const int columns = (int)(panelWidth / cellSize);

		if (columns == 0)
		{
			ImGui::End();
			return;
		}

		ImGui::Columns(columns, 0, false);

		const bool leftMouseButtonDoubleClicked = ImGui::IsMouseDoubleClicked(GLFW_MOUSE_BUTTON_1);

		const ImTextureID folderIconId = (ImTextureID)TextureManager::GetInstance().GetByName("FolderIcon")->GetRendererID();
		const ImTextureID fileIconId = (ImTextureID)TextureManager::GetInstance().GetByName("FileIcon")->GetRendererID();
		const ImTextureID metaIconId = (ImTextureID)TextureManager::GetInstance().GetByName("MetaIcon")->GetRendererID();
		const ImTextureID materialIconId = (ImTextureID)TextureManager::GetInstance().GetByName("MaterialIcon")->GetRendererID();

		bool iconHovered = false;

		for (auto& directoryIter : std::filesystem::directory_iterator(m_CurrentDirectory))
		{
			const std::string path = Utils::Replace(directoryIter.path().string(), '\\', '/');
			if (Utils::Contains(path, ".cpp") || Utils::Contains(path, ".h"))
			{
				continue;
			}

			const std::string filename = Utils::RemoveDirectoryFromFilePath(path);
			const std::string format = Utils::GetResolutionFromFilePath(path);
			ImTextureID currentIcon;

			if (m_FilterBuffer[0] != '\0' && !Utils::Contains(filename, m_FilterBuffer))
			{
				continue;
			}

			if (directoryIter.is_directory())
			{
				currentIcon = folderIconId;
			}
			else if (format == "mat")
			{
				currentIcon = materialIconId;
			}
			else if (format == "meta")
			{
				currentIcon = metaIconId;
			}
			else if (Utils::IsTextureFile(path))
			{
				if (Texture* texture = TextureManager::GetInstance().GetByFilePath(path))
				{
					currentIcon = (ImTextureID)texture->GetRendererID();
				}
				else
				{
					currentIcon = fileIconId;
				}
			}
			else
			{
				currentIcon = fileIconId;
			}

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
			ImGui::PushID(filename.c_str());
			ImGui::ImageButton(currentIcon, { thumbnailSize, thumbnailSize }, ImVec2(0, 1), ImVec2(1, 0));
			ImGui::PopID();

			iconHovered = iconHovered ? iconHovered : ImGui::IsItemHovered();

			std::string fileType = Utils::GetResolutionFromFilePath(path);

			if (leftMouseButtonDoubleClicked && ImGui::IsItemHovered())
			{
				if (fileType == "anim")
				{
					m_Animation2DMenu.m_IsActive = true;
					m_Animation2DMenu.m_Animation = Animation2DManager::GetInstance().Load(path);
				}
				else if (fileType == "ta")
				{
					m_TextureAtlasMenu.m_IsActive = true;
					m_TextureAtlasMenu.m_TextureAtlas = Serializer::DeSerializeTextureAtlas(path);
				}
			}

			if (ImGui::BeginPopupContextItem())
			{
				if (ImGui::MenuItem("Delete"))
				{
					m_DeleteDirectoryOrFile.m_FilePath = directoryIter.path();
					m_DeleteDirectoryOrFile.m_FileName = filename;
					m_DeleteDirectoryOrFile.m_IsActive = true;
				}

				if (fileType == "prefab" && ImGui::MenuItem("Update"))
				{
					for (auto gameObject : m_CurrentScene->m_GameObjects)
					{
						if (Utils::Replace(gameObject->m_PrefabFilePath, '\\', '/') == path)
						{
							gameObject->ResetWithPrefab();
						}
					}
				}
				else if (fileType == "obj" || fileType == "fbx" || fileType == "dae")
				{
					if (ImGui::MenuItem("Generate all mesh meta"))
					{
						MeshManager::GetInstance().GenerateMeshMeta(path);
					}
					else if (ImGui::MenuItem("Generate missing mesh meta"))
					{
						MeshManager::GetInstance().GenerateMeshMeta(path, true);
					}
				}

				ImGui::EndPopup();
			}

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

		if (ImGui::IsWindowHovered() && !iconHovered && Input::Mouse::IsMousePressed(Keycode::MOUSE_BUTTON_2))
		{
			m_CreatingFileMenu.m_FilePath = m_CurrentDirectory;
			m_CreatingFileMenu.m_IsActive = true;
		}

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

void Editor::Properties()
{
	if (ImGui::Begin("Properties"))
	{
		for(const std::string gameObjectUUID : m_SelectedGameObjects)
		{
			GameObject* gameObject = m_CurrentScene->FindGameObjectByUUID(gameObjectUUID);
			if (!gameObject)
			{
				continue;
			}

			ComponentsMenu(gameObject);

			ImGui::Text("UUID: %s", gameObject->GetUUID().c_str());

			char name[64];
			strcpy(name, gameObject->GetName().c_str());
			if (ImGui::InputText("Name", name, sizeof(name)))
			{
				gameObject->SetName(name);
			}

			ImGui::Text("Owner: %s", gameObject->HasOwner() ? gameObject->GetOwner()->GetName().c_str() : "Null");
			ImGui::Checkbox("Is Serializable", &gameObject->m_IsSerializable);
			ImGui::Checkbox("Is Selectable", &gameObject->m_IsSelectable);
			ImGui::Text(gameObject->IsInitialized() ? "Is Initialized: Yes" : "Initialized: No");

			if (gameObject->IsPrefab())
			{
				if (ImGui::CollapsingHeader(std::string("Prefab: " + gameObject->m_PrefabFilePath).c_str()))
				{
					if (ImGui::Button("Update"))
					{
						gameObject->UpdatePrefab();
					}

					ImGui::SameLine();

					if (ImGui::Button("Reset"))
					{
						gameObject->ResetWithPrefab();
					}
				}
			}

			TransformComponent(gameObject->m_Transform);
			Renderer2DComponent(gameObject);
			Rigidbody2DComponent(gameObject);
			Rigidbody3DComponent(gameObject);
			BoxCollider2DComponent(gameObject);
			BoxCollider3DComponent(gameObject);
			CircleCollider2DComponent(gameObject);
			SphereCollider3DComponent(gameObject);
			Animator2DComponent(gameObject);
			ParticleEmitterComponent(gameObject);
			ScriptComponent(gameObject);
			SpotLightComponent(gameObject);
			PointLightComponent(gameObject);
			DirectionalLightComponent(gameObject);
			Renderer3DComponent(gameObject);
			SplineComponent(gameObject);

			UserDefinedComponents(gameObject);

			ImGui::NewLine();

			if (Environment::GetInstance().GetMainCamera()->GetType() == Camera::CameraType::ORTHOGRAPHIC)
			{
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
			else if (Environment::GetInstance().GetMainCamera()->GetType() == Camera::CameraType::PERSPECTIVE)
			{
				if (Renderer3D* r3d = gameObject->m_ComponentManager.GetComponent<Renderer3D>())
				{
					if (r3d->m_Mesh)
					{
						Visualizer::DrawWireFrameCube(gameObject->m_Transform.GetPositionMat4(),
							gameObject->m_Transform.GetRotationMat4(Transform::System::LOCAL),
							gameObject->m_Transform.GetScaleMat4(Transform::System::LOCAL),
							r3d->m_Mesh->m_BoundingBox.m_Min, r3d->m_Mesh->m_BoundingBox.m_Max);
					}
				}
			}
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
		Pengine::Environment& environment = Environment::GetInstance();
		
		char name[64];
		strcpy(name, m_CurrentScene->m_Title.c_str());
		if (ImGui::InputText("Scene title", name, sizeof(name)))
		{
			m_CurrentScene->m_Title = name;
		}

		if (ImGui::Checkbox("Vsync", &Window::GetInstance().m_VSync))
		{
			Window::GetInstance().SetVSyncEnabled(Window::GetInstance().m_VSync);
		}

		if (ImGui::Checkbox("Depth Test", &environment.m_DepthTest))
		{
			environment.SetDepthTest(environment.m_DepthTest);
		}

		const char* windowModes[] = { "Windowed", "Fullscreen" };
		if (ImGui::Combo("Window mode", &m_SelectedWindowMode, windowModes, 2))
		{
			Window::GetInstance().SetWindowMode((Window::WindowMode)m_SelectedWindowMode);
		}

		if (ImGui::CollapsingHeader("Post processing"))
		{
			Indent indent;

			ImGui::SliderFloat("Gamma", &environment.m_Gamma, 0.0f, 5.0);

			ImGui::PushID("SSAO settings");

			if (ImGui::CollapsingHeader("SSAO"))
			{
				Indent indent;

				ImGui::Checkbox("Is Enabled", &environment.m_SSAO.m_IsEnabled);
				ImGui::SliderFloat("Radius", &environment.m_SSAO.m_Radius, 0.0f, 5.0f);
				ImGui::SliderFloat("Bias", &environment.m_SSAO.m_Bias, 0.0f, 0.1f);
				if (ImGui::SliderInt("Kernel Size", &environment.m_SSAO.m_KernelSize, 2, 64))
				{
					Renderer::GetInstance().GenerateSSAOKernel();
				}

				if (ImGui::SliderInt("Noise Size", &environment.m_SSAO.m_NoiseTextureDimension, 4, 64))
				{
					Renderer::GetInstance().GenerateSSAONoiseTexture();
				}

				if (ImGui::CollapsingHeader("Blur"))
				{
					Indent indent;

					ImGui::SliderInt("Pixels Blured", &environment.m_SSAO.m_Blur.m_PixelsBlured, 1, 6);
					ImGui::SliderInt("Passes", &environment.m_SSAO.m_Blur.m_BlurPasses, 1, 10);
				}
			}

			ImGui::PopID();

			ImGui::PushID("Bloom settings");

			if (ImGui::CollapsingHeader("Bloom"))
			{
				Indent indent;

				ImGui::Checkbox("Is Enabled", &environment.m_BloomSettings.m_IsEnabled);
				ImGui::SliderFloat("Threshold", &environment.m_BloomSettings.m_BrightnessThreshold, 0.0f, 2.0f);
				ImGui::SliderFloat("Exposure", &environment.m_BloomSettings.m_Exposure, 0.0f, 5.0);
				if (ImGui::CollapsingHeader("Blur"))
				{
					Indent indent;

					ImGui::SliderInt("Pixels", &environment.m_BloomSettings.m_PixelsBlured, 1, 6);
					ImGui::SliderInt("Blur passes", &environment.m_BloomSettings.m_BlurPasses, 3, 20);
				}
			}

			ImGui::PopID();
		}

		ImGui::PushID("Shadows settings");

		if (ImGui::CollapsingHeader("Shadows"))
		{
			Indent indent;

			ImGui::Checkbox("Is Enabled", &environment.m_ShadowsSettings.m_IsEnabled);
			ImGui::Checkbox("Is Visualized", &environment.m_ShadowsSettings.m_IsVisualized);
			ImGui::SliderInt("Max Point Light Shadows", &environment.m_ShadowsSettings.m_MaxPointLightShadows, 1, Renderer::GetInstance().m_PointLights.m_MaxShadowsSize);
			ImGui::SliderInt("Max Spot Light Shadows", &environment.m_ShadowsSettings.m_MaxSpotLightShadows, 1, Renderer::GetInstance().m_SpotLights.m_MaxShadowsSize);

			if(ImGui::CollapsingHeader("Blur"))
			{
				Indent indent;

				ImGui::SliderInt("Pixels Blured", &environment.m_ShadowsSettings.m_Blur.m_PixelsBlured, 1, 6);
				ImGui::SliderInt("Passes", &environment.m_ShadowsSettings.m_Blur.m_BlurPasses, 1, 10);
			}
		}

		ImGui::PopID();

		ImGui::PushID("Main camera settings");

		if (ImGui::CollapsingHeader("Main camera"))
		{
			Indent indent;

			std::shared_ptr<Camera> camera = environment.GetMainCamera();

			m_SelectedCameraType = (int)camera->GetType();
			const char* cameraTypes[] = { "Orthographic", "Perspective" };
			if (ImGui::Combo("Camera type", &m_SelectedCameraType, cameraTypes, 2))
			{
				camera->SetType((Camera::CameraType)m_SelectedCameraType);
			}

			ImGui::DragFloat("Speed", &camera->m_Speed, 1.0f, 0.0f, 100.0f);
			ImGui::DragFloat("Zoom sensetivity", &camera->m_ZoomSensetivity, 0.1f, 0.1f, 5.0f);
			if (ImGui::SliderAngle("Fov", &camera->m_Fov, 0.0f, 90.0f))
			{
				camera->UpdateProjection();
			}
			if (ImGui::SliderFloat("Z far", &camera->m_Zfar, 0.0f, 10000.0f))
			{
				camera->UpdateProjection();
			}
			if (ImGui::SliderFloat("Z near", &camera->m_Znear, 0.0f, 1.0f))
			{
				camera->UpdateProjection();
			}

			TransformComponent(camera->m_Transform);
		}

		ImGui::PopID();

		ImGui::End();
	}
}

void Editor::ToolBar()
{
	if (ImGui::Begin("##toolBar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
	{
		const float size = ImGui::GetWindowHeight() - 4.0f;
		ImTextureID icon = EntryPoint::GetApplication().GetState() == Application::ApplicationState::Edit ?
			(ImTextureID)TextureManager::GetInstance().GetByName("PlayButton")->GetRendererID()
			: (ImTextureID)TextureManager::GetInstance().GetByName("StopButton")->GetRendererID();
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
		Indent indent;

		ImGui::Checkbox("Follow owner", &transform.m_FollowOwner);
		glm::vec3 position = transform.GetPosition(Transform::System::LOCAL);
		glm::vec3 rotation = glm::degrees(transform.GetRotation(Transform::System::LOCAL));
		glm::vec3 scale = transform.GetScale(Transform::System::LOCAL);
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
		if (RemoveComponentMenu(gameObject, r2d))
		{
			if (ImGui::CollapsingHeader("Renderer2D"))
			{
				Indent indent;

				int layer = r2d->GetLayer();
				if (ImGui::SliderInt("Layer", &layer, 0, 9))
				{
					r2d->SetLayer(layer);
				}

				ImGui::SliderFloat("Ambient", &r2d->m_Ambient, 0.0f, 5.0f);

				ImGui::ColorEdit4("Color", &r2d->m_Color[0]);

				std::vector<float> uv = r2d->GetUV();

				ImGui::Image((ImTextureID)r2d->GetTexture()->GetRendererID(), { 64.0f, 64.0f }, ImVec2(uv[0], uv[5]), ImVec2(uv[4], uv[1]));

				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSETS_BROWSER_ITEM"))
					{
						std::string path((const char*)payload->Data);
						path.resize(payload->DataSize);
						TextureManager::GetInstance().AsyncLoad(path,
							[=](Texture* texture) 
						{
							r2d->SetTexture(texture);
						});
					}
					ImGui::EndDragDropTarget();
				}

				ImGui::SameLine();

				ImGui::PushID(r2d->GetTexture()->GetRendererID() + 100000);
				if (ImGui::Button("Reset"))
				{
					r2d->SetTexture(TextureManager::GetInstance().White());
				}
				ImGui::PopID();
				
				ImGui::Image((ImTextureID)r2d->GetNormalTexture()->GetRendererID(), { 64.0f, 64.0f }, ImVec2(uv[0], uv[5]), ImVec2(uv[4], uv[1]));

				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSETS_BROWSER_ITEM"))
					{
						std::string path((const char*)payload->Data);
						path.resize(payload->DataSize);
						TextureManager::GetInstance().AsyncLoad(path,
							[=](Texture* texture) 
						{
							r2d->SetNormalTexture(texture);
						});
					}
					ImGui::EndDragDropTarget();
				}

				ImGui::SameLine();

				ImGui::PushID(r2d->GetNormalTexture()->GetRendererID() + 100001);
				if (ImGui::Button("Reset"))
				{
					r2d->SetNormalTexture(TextureManager::GetInstance().White());
				}
				ImGui::PopID();

				ImGui::SameLine();

				ImGui::PushID(r2d->GetNormalTexture()->GetRendererID() + 100002);
				ImGui::Checkbox("Use", &r2d->m_IsNormalUsed);
				ImGui::PopID();

				ImGui::Image((ImTextureID)r2d->GetEmissiveMaskTexture()->GetRendererID(), { 64.0f, 64.0f }, ImVec2(uv[0], uv[5]), ImVec2(uv[4], uv[1]));

				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSETS_BROWSER_ITEM"))
					{
						std::string path((const char*)payload->Data);
						path.resize(payload->DataSize);
						TextureManager::GetInstance().AsyncLoad(path,
							[=](Texture* texture) 
						{
							r2d->SetEmissiveMaskTexture(texture);
						});
					}
					ImGui::EndDragDropTarget();
				}

				ImGui::SameLine();

				ImGui::PushID(r2d->GetEmissiveMaskTexture()->GetRendererID() + 100003);
				if (ImGui::Button("Reset"))
				{
					r2d->SetEmissiveMaskTexture(TextureManager::GetInstance().White());
				}
				ImGui::PopID();

				ImGui::SameLine();

				ImGui::PushID(r2d->GetEmissiveMaskTexture()->GetRendererID() + 100004);
				ImGui::SliderFloat("Intensity", &r2d->m_EmmisiveMaskIntensity, 0.0f, 10.0f);
				ImGui::PopID();

				ImGui::SliderFloat("Outer radius", &r2d->m_OuterRadius, 0.0f, 10.0f);
				ImGui::SliderFloat("Inner radius", &r2d->m_InnerRadius, 0.0f, r2d->m_OuterRadius);
				r2d->m_InnerRadius = glm::clamp(r2d->m_InnerRadius, 0.0f, r2d->m_OuterRadius);

				if (ImGui::Button("Reset UV"))
				{
					r2d->OriginalUV();
				}
			}
		}
	}
}

void Editor::BoxCollider2DComponent(GameObject* gameObject)
{
	BoxCollider2D* bc2d = gameObject->m_ComponentManager.GetComponent<BoxCollider2D>();
	if (bc2d)
	{
		if (RemoveComponentMenu(gameObject, bc2d))
		{
			if (ImGui::CollapsingHeader("BoxCollider2D"))
			{
				Indent indent;

				glm::vec2 size = bc2d->GetSize();
				ImGui::SliderFloat2("Size", &size[0], 0.0f, 10.0f);
				bc2d->SetSize(size);

				glm::vec2 offset = bc2d->GetOffset();
				ImGui::SliderFloat2("Offset", &offset[0], -10.0f, 10.0f);

				float density = bc2d->GetDensity();
				ImGui::DragFloat("Density", &density, 0.1f, 0.0f, 5.0f);
				bc2d->SetDensity(density);

				float friction = bc2d->GetFriction();
				ImGui::DragFloat("Friction", &friction, 0.1f, 0.0f, 5.0f);
				bc2d->SetFriction(friction);

				float restitution = bc2d->GetRestitution();
				ImGui::DragFloat("Restitution", &restitution, 0.1f, 0.0f, 5.0f);
				bc2d->SetRestitution(restitution);

				float restitutionThreshold = bc2d->GetRestitutionThreshold();
				ImGui::DragFloat("Restitution threshold", &restitutionThreshold, 0.1f, 0.0f, 5.0f);
				bc2d->SetRestitutionThreshold(restitutionThreshold);

				char tag[64];
				strcpy(tag, bc2d->GetTag().c_str());
				ImGui::InputText("Tag", tag, sizeof(char) * 32);
				bc2d->SetTag(tag);
				
				bool isTrigger = bc2d->IsTrigger();
				ImGui::Checkbox("Trigger", &isTrigger);
				bc2d->SetTrigger(isTrigger);
			}
		}
	}
}

void Editor::BoxCollider3DComponent(GameObject* gameObject)
{
	BoxCollider3D* bc3d = gameObject->m_ComponentManager.GetComponent<BoxCollider3D>();
	if (bc3d)
	{
		if (RemoveComponentMenu(gameObject, bc3d))
		{
			if (ImGui::CollapsingHeader("BoxCollider3D"))
			{
				Indent indent;

				glm::vec3 offset = bc3d->GetOffset();
				if (DrawVec3Control("Offset", offset, 0.0f))
				{
					bc3d->SetOffset(offset);
				}

				glm::vec3 halfExtent = bc3d->GetHalfExtent();
				if (DrawVec3Control("Half Extent", halfExtent, 1.0f, { 0.001f, 25.0f }))
				{
					bc3d->SetHalfExtent(halfExtent);
				}
			}
		}
	}
}

void Editor::CircleCollider2DComponent(GameObject* gameObject)
{
	CircleCollider2D* cc2d = gameObject->m_ComponentManager.GetComponent<CircleCollider2D>();
	if (cc2d)
	{
		if (RemoveComponentMenu(gameObject, cc2d))
		{
			if (ImGui::CollapsingHeader("CircleCollider2D"))
			{
				Indent indent;

				float radius = cc2d->GetRadius();
				ImGui::SliderFloat("Radius", &radius, 0.0, 10.0f);
				cc2d->SetRadius(radius);

				glm::vec2 offset = cc2d->GetOffset();
				ImGui::SliderFloat2("Offset", &offset[0], -10.0f, 10.0f);

				float density = cc2d->GetDensity();
				ImGui::DragFloat("Density", &density, 0.1f, 0.0f, 5.0f);
				cc2d->SetDensity(density);

				float friction = cc2d->GetFriction();
				ImGui::DragFloat("Friction", &friction, 0.1f, 0.0f, 5.0f);
				cc2d->SetFriction(friction);

				float restitution = cc2d->GetRestitution();
				ImGui::DragFloat("Restitution", &restitution, 0.1f, 0.0f, 5.0f);
				cc2d->SetRestitution(restitution);

				float restitutionThreshold = cc2d->GetRestitutionThreshold();
				ImGui::DragFloat("Restitution threshold", &restitutionThreshold, 0.1f, 0.0f, 5.0f);
				cc2d->SetRestitutionThreshold(restitutionThreshold);

				char tag[64];
				strcpy(tag, cc2d->GetTag().c_str());
				ImGui::InputText("Tag", tag, sizeof(char) * 32);
				cc2d->SetTag(tag);

				bool isTrigger = cc2d->IsTrigger();
				ImGui::Checkbox("Trigger", &isTrigger);
				cc2d->SetTrigger(isTrigger);
			}
		}
	}
}

void Editor::SphereCollider3DComponent(GameObject* gameObject)
{
	SphereCollider3D* sc3d = gameObject->m_ComponentManager.GetComponent<SphereCollider3D>();
	if (sc3d)
	{
		if (RemoveComponentMenu(gameObject, sc3d))
		{
			if (ImGui::CollapsingHeader("SphereCollider3D"))
			{
				Indent indent;

				glm::vec3 offset = sc3d->GetOffset();
				if (DrawVec3Control("Offset", offset, 0.0f))
				{
					sc3d->SetOffset(offset);
				}

				float radius = sc3d->GetRadius();
				ImGui::SliderFloat("Radius", &radius, 0.0, 10.0f);
				sc3d->SetRadius(radius);
			}
		}
	}
}

void Editor::Rigidbody2DComponent(GameObject* gameObject)
{
	Rigidbody2D* rb2d = gameObject->m_ComponentManager.GetComponent<Rigidbody2D>();
	if (rb2d)
	{
		if (RemoveComponentMenu(gameObject, rb2d))
		{
			if (ImGui::CollapsingHeader("Rigidbody2D"))
			{
				Indent indent;

				if (ImGui::Checkbox("Fixed rotation", &rb2d->m_FixedRotation))
				{
					rb2d->SetFixedRotation(rb2d->m_FixedRotation);
				}

				bool type = rb2d->IsStatic();
				if (ImGui::Checkbox("Type Static", &type))
				{
					rb2d->SetStatic(type);
				}
			}
		}
	}
}

void Editor::Rigidbody3DComponent(GameObject* gameObject)
{
	Rigidbody3D* rb3d = gameObject->m_ComponentManager.GetComponent<Rigidbody3D>();
	if (rb3d)
	{
		if (RemoveComponentMenu(gameObject, rb3d))
		{
			if (ImGui::CollapsingHeader("Rigidbody3D"))
			{
				Indent indent;

				ImGui::SliderFloat("Mass", &rb3d->m_Mass, 0.0f, 10.0f);
				ImGui::SliderFloat("Restitution", &rb3d->m_Restitution, 0.0f, 2.0f);
				ImGui::SliderFloat("Friction", &rb3d->m_Friction, 0.0f, 1.0f);
				ImGui::Checkbox("AllowToSleep", &rb3d->m_AllowToSleep);
			}
		}
	}
}

void Editor::Animator2DComponent(GameObject* gameObject)
{
	Animator2D* a2d = gameObject->m_ComponentManager.GetComponent<Animator2D>();
	if (a2d)
	{
		if (RemoveComponentMenu(gameObject, a2d))
		{
			if (ImGui::CollapsingHeader("Animator 2D"))
			{
				Indent indent;

				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSETS_BROWSER_ITEM"))
					{
						std::string path((const char*)payload->Data);
						path.resize(payload->DataSize);

						if (Animation2DManager::Animation2D* animation =
							Animation2DManager::GetInstance().Load(path))
						{
							a2d->AddAnimation(animation);
						}
					}
					ImGui::EndDragDropTarget();
				}

				for (auto animation : a2d->GetAnimations())
				{
					
					ImGui::PushID(animation);
					if (ImGui::Button("-"))
					{
						a2d->RemoveAnimation(animation);
					}
					ImGui::PopID();

					ImGui::SameLine();

					ImGuiStyle* style = &ImGui::GetStyle();
					ImVec4 tempNameColor = style->Colors[ImGuiCol_Text];

					if (a2d->GetCurrentAnimation() == animation)
					{
						style->Colors[ImGuiCol_Text] = ImVec4(0.0f, 0.8f, 1.0f, 1.0f);
					}

					if (ImGui::Button(animation->GetName().c_str()))
					{
						a2d->SetCurrentAnimation(animation);
					}

					style->Colors[ImGuiCol_Text] = tempNameColor;
				}

				if (ImGui::Button("Clear"))
				{
					a2d->RemoveAllAnimations();
				}

				float animationSpeed = a2d->GetSpeed();
				if (ImGui::InputFloat("Speed", &animationSpeed, 0.1f, 1.0f))
				{
					a2d->SetSpeed(animationSpeed);
				}

				const float size = 32.0f;
				ImTextureID icon = a2d->IsPlaying() ?
					(ImTextureID)TextureManager::GetInstance().GetByName("StopButton")->GetRendererID()
					: (ImTextureID)TextureManager::GetInstance().GetByName("PlayButton")->GetRendererID();
				if (ImGui::ImageButton(icon, { size, size }))
				{
					if (a2d->IsPlaying())
					{
						a2d->SetPlay(false);
					}
					else
					{
						a2d->SetPlay(true);
					}

				}
			}
		}
	}
}

void Editor::ParticleEmitterComponent(GameObject* gameObject)
{
	ParticleEmitter* particleEmitter = gameObject->m_ComponentManager.GetComponent<ParticleEmitter>();
	if (particleEmitter)
	{
		if (RemoveComponentMenu(gameObject, particleEmitter))
		{
			if (ImGui::CollapsingHeader("Particle emitter"))
			{
				Indent indent;

				int facingMode = (int)particleEmitter->m_FacingMode;
				const char* facingModes[] = { "Default", "Spherical", "Cylindrical" };
				if (ImGui::Combo("Camera type", &facingMode, facingModes, 3))
				{
					particleEmitter->m_FacingMode = (ParticleEmitter::FacingMode)facingMode;
				}

				ImGui::ColorEdit4("ColorStart", &(particleEmitter->m_ColorStart[0]));
				ImGui::ColorEdit4("ColorEnd", &(particleEmitter->m_ColorEnd[0]));
				ImGui::Checkbox("Loop", &particleEmitter->m_Loop);
				ImGui::SliderInt("Amount", &particleEmitter->m_ParticlesSize, 1, 100);
				ImGui::SliderFloat("Intensity", &particleEmitter->m_Intensity, 0.0f, 5.0f);
				ImGui::SliderFloat("Acceleration", &(particleEmitter->m_Acceleration), 0.0f, 10.0f);
				ImGui::SliderFloat("Gravity", &(particleEmitter->m_Gravity.y), -10.0f, 10.0f);
				ImGui::SliderFloat("Max time to live", &(particleEmitter->m_MaxTimeToLive), 0.0f, 10.0f);
				ImGui::SliderFloat2("Scale", &(particleEmitter->m_Scale[0]), 0.0f, 5.0f);
				ImGui::SliderFloat2("Time to spawn", &(particleEmitter->m_TimeToSpawn[0]), 0.0f, 10.0f);
				ImGui::SliderFloat2("Dir x", &(particleEmitter->m_Direction[0].x), -1.0f, 1.0f);
				ImGui::SliderFloat2("Dir y", &(particleEmitter->m_Direction[1].x), -1.0f, 1.0f);
				ImGui::SliderFloat2("Dir z", &(particleEmitter->m_Direction[2].x), -1.0f, 1.0f);
				ImGui::SliderFloat2("Pos X", &(particleEmitter->m_RadiusToSpawn[0].x), -5.0f, 5.0f);
				ImGui::SliderFloat2("Pos Y", &(particleEmitter->m_RadiusToSpawn[1].x), -5.0f, 5.0f);
				ImGui::SliderFloat2("Pos Z", &(particleEmitter->m_RadiusToSpawn[2].x), -5.0f, 5.0f);

				ImGui::Image((ImTextureID)particleEmitter->m_Texture->GetRendererID(), { 128.0f, 128.0f }, ImVec2(0, 1), ImVec2(1, 0));

				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSETS_BROWSER_ITEM"))
					{
						std::string path((const char*)payload->Data);
						path.resize(payload->DataSize);
						TextureManager::GetInstance().AsyncLoad(path,
							[=](Texture* texture) 
						{
							particleEmitter->m_Texture = texture;
						});
					}
					ImGui::EndDragDropTarget();
				}

				if (ImGui::Button("WhiteTexture"))
				{
					particleEmitter->m_Texture = TextureManager::GetInstance().White();
				}
			}
		}
	}
}

void Editor::ScriptComponent(GameObject* gameObject)
{
Script* script = gameObject->m_ComponentManager.GetComponent<Script>();
if (script)
{
	if (RemoveComponentMenu(gameObject, script))
	{
		if (ImGui::CollapsingHeader("Script"))
		{
			Indent indent;

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSETS_BROWSER_ITEM"))
				{
					std::string path((const char*)payload->Data);
					path.resize(payload->DataSize);
					if (Utils::Contains(path, ".lua"))
					{
						script->Assign(path);
					}
				}
				ImGui::EndDragDropTarget();
			}

			for (auto& state : script->m_LStates)
			{
				if (ImGui::Button(state->m_FilePath.c_str()))
				{
					script->Remove(state->m_FilePath);
				}
			}
		}
	}
}
}

void Editor::PointLightComponent(GameObject* gameObject)
{
	PointLight* pointLight = gameObject->m_ComponentManager.GetComponent<PointLight>();
	if (pointLight)
	{
		if (RemoveComponentMenu(gameObject, pointLight))
		{
			if (ImGui::CollapsingHeader("Point Light"))
			{
				Indent indent;

				ImGui::SliderFloat("Constant", &pointLight->m_Constant, 0.0f, 1.0f);
				ImGui::SliderFloat("Linear", &pointLight->m_Linear, 0.0f, 0.100f);
				ImGui::SliderFloat("Quadratic", &pointLight->m_Quadratic, 0.0f, 0.1f);
				ImGui::ColorEdit3("Color", &pointLight->m_Color[0]);
				
				if (ImGui::SliderFloat("ZNear", &pointLight->m_ZNear, 0.1, 1.0f))
				{
					pointLight->BuildProjectionMatrix();
				}

				if (ImGui::SliderFloat("ZFar", &pointLight->m_ZFar, 1.0f, 150.0f))
				{
					pointLight->BuildProjectionMatrix();
				}

				ImGui::SliderFloat("Fog", &pointLight->m_Fog, 0.0f, 1.0f);

				float shadowBias = pointLight->GetShadowBias();
				if (ImGui::SliderFloat("Bias", &shadowBias, 0.0f, 1.0f, "%.6f"))
				{
					pointLight->SetShadowBias(shadowBias);
				}

				bool drawShadows = pointLight->IsDrawShadows();
				if (ImGui::Checkbox("Draw Shadows", &drawShadows))
				{
					pointLight->SetDrawShadows(drawShadows);
				}
			}
		}
	}
}

void Editor::SpotLightComponent(GameObject* gameObject)
{
	SpotLight* spotLight = gameObject->m_ComponentManager.GetComponent<SpotLight>();
	if (spotLight)
	{
		if (RemoveComponentMenu(gameObject, spotLight))
		{
			if (ImGui::CollapsingHeader("Spot Light"))
			{
				Indent indent;

				ImGui::SliderFloat("Constant", &spotLight->m_Constant, 0.0f, 1.0f);
				ImGui::SliderFloat("Linear", &spotLight->m_Linear, 0.0f, 0.100f);
				ImGui::SliderFloat("Quadratic", &spotLight->m_Quadratic, 0.0f, 0.1f);

				float innerCutOff = spotLight->GetInnerCutOff();
				if (ImGui::SliderAngle("Inner CutOff", &innerCutOff, 0.0f, 180.0f))
				{
					spotLight->SetInnerCutOff(innerCutOff);
				}

				float outerCutOff = spotLight->GetOuterCutOfff();
				if (ImGui::SliderAngle("Outer CutOff", &outerCutOff, 0.0f, 180.0f))
				{
					spotLight->SetOuterCutOff(outerCutOff);
				}

				ImGui::ColorEdit3("Color", &spotLight->m_Color[0]);

				if (ImGui::SliderFloat("ZNear", &spotLight->m_ZNear, 0.1, 1.0f))
				{
					spotLight->BuildProjectionMatrix();
				}

				if (ImGui::SliderFloat("ZFar", &spotLight->m_ZFar, 1.0f, 150.0f))
				{
					spotLight->BuildProjectionMatrix();
				}

				ImGui::SliderFloat("Fog", &spotLight->m_Fog, 0.0f, 1.0f);

				float shadowBias = spotLight->GetShadowBias();
				if (ImGui::SliderFloat("Bias", &shadowBias, 0.0f, 0.001f, "%.6f"))
				{
					spotLight->SetShadowBias(shadowBias);
				}

				int shadowPcf = spotLight->GetShadowPcf();
				if (ImGui::SliderInt("Pcf", &shadowPcf, 0, 5))
				{
					spotLight->SetShadowPcf(shadowPcf);
				}
				
				bool drawShadows = spotLight->IsDrawShadows();
				if (ImGui::Checkbox("Draw Shadows", &drawShadows))
				{
					spotLight->SetDrawShadows(drawShadows);
				}
			}
		}
	}
}

void Editor::DirectionalLightComponent(GameObject* gameObject)
{
	DirectionalLight* directionalLight = gameObject->m_ComponentManager.GetComponent<DirectionalLight>();
	if (directionalLight)
	{
		if (RemoveComponentMenu(gameObject, directionalLight))
		{
			if (ImGui::CollapsingHeader("Directional Light "))
			{
				Indent indent;

				float intensity = directionalLight->GetIntensity();
				if (ImGui::SliderFloat("Intensity", &intensity, 0.0f, 10.0f))
				{
					directionalLight->SetIntensity(intensity);
				}

				glm::vec3 color = directionalLight->GetColor();
				if (ImGui::ColorEdit3("Color", &color[0]))
				{
					directionalLight->SetColor(color);
				}

				std::vector<float> bias = directionalLight->GetBias();
				for (size_t i = 0; i < bias.size(); i++)
				{
					if (ImGui::SliderFloat(std::string("Bias" + std::to_string(i)).c_str(), &bias[i], 0.001f, 0.005f, "%.4f"))
					{
						directionalLight->SetBias(bias);
					}
				}

				int pcf = directionalLight->GetPcf();
				if (ImGui::SliderInt("Pcf", &pcf, 0, 10))
				{
					directionalLight->SetPcf(pcf);
				}

				float fog = directionalLight->GetFog();
				if (ImGui::SliderFloat("Fog", &fog, 0.0f, 1.0f))
				{
					directionalLight->SetFog(fog);
				}

				float texelSize = directionalLight->GetTexels();
				if (ImGui::SliderFloat("Texel size", &texelSize, 0.0f, 2.0f))
				{
					directionalLight->SetTexels(texelSize);
				}

				float zFarScale = directionalLight->GetZFarScale();
				if (ImGui::SliderFloat("Z far scale", &zFarScale, 0.0f, 2.0f))
				{
					directionalLight->SetZFarScale(zFarScale);
				}

				const float zNear = Environment::GetInstance().GetMainCamera()->GetZNear();
				const float zFar = Environment::GetInstance().GetMainCamera()->GetZFar();
				std::vector<float> cascadesDistance = directionalLight->GetCascadesDistance();
				for (size_t i = 0; i < cascadesDistance.size(); ++i)
				{
					if (i == 0)
					{
						ImGui::SliderFloat(std::string("Cascade " + std::to_string(i)).c_str(), &cascadesDistance[i],
							zNear,
							cascadesDistance[i + 1]);
					}
					else if (i < cascadesDistance.size() + 1)
					{
						ImGui::SliderFloat(std::string("Cascade " + std::to_string(i)).c_str(), &cascadesDistance[i],
							cascadesDistance[i - 1],
							zFar * directionalLight->GetZFarScale());
					}
					else if (i < cascadesDistance.size())
					{
						ImGui::SliderFloat(std::string("Cascade " + std::to_string(i)).c_str(), &cascadesDistance[i],
							cascadesDistance[i - 1],
							cascadesDistance[i + 1]);
					}
				}

				directionalLight->SetCascadesDistance(cascadesDistance);

				ImGui::Text(std::string("Cascade " + std::to_string(cascadesDistance.size() + 1) + " : %f").c_str(),
					zFar * directionalLight->GetZFarScale());
			}
		}
	}
}

void Editor::Renderer3DComponent(GameObject* gameObject)
{
	size_t imageOffset = 1000;

	Renderer3D* r3d = gameObject->m_ComponentManager.GetComponent<Renderer3D>();
	if (r3d)
	{
		if (RemoveComponentMenu(gameObject, r3d))
		{
			if (ImGui::CollapsingHeader("Renderer3D"))
			{
				Indent indent;

				auto lodSection = [r3d](const std::string& label, size_t lod)
				{
					if (ImGui::CollapsingHeader(label.c_str()))
					{
						Indent indent;

						if (ImGui::BeginDragDropTarget())
						{
							if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSETS_BROWSER_ITEM"))
							{
								std::string path((const char*)payload->Data);
								path.resize(payload->DataSize);

								if (Utils::Contains(path, ".meta"))
								{
									MeshManager::GetInstance().LoadAsync(path,
										[r3d, lod](Mesh* mesh)
									{
										r3d->SetMesh(mesh, lod);
									});
								}
							}

							ImGui::EndDragDropTarget();
						}

						Mesh* mesh = r3d->m_Lods[lod];

						ImGui::Text("FilePath: %s", mesh ? mesh->GetFilePath().c_str() : "None");
						ImGui::Text("Name: %s", mesh ? mesh->GetName().c_str() : "None");
						
						if (mesh)
						{
							ImGui::Text("Min: %f %f %f", mesh->m_BoundingBox.m_Min.x, mesh->m_BoundingBox.m_Min.y, mesh->m_BoundingBox.m_Min.z);
							ImGui::Text("Max: %f %f %f", mesh->m_BoundingBox.m_Max.x, mesh->m_BoundingBox.m_Max.y, mesh->m_BoundingBox.m_Max.z);

							if (ImGui::Checkbox("Cull Face", &mesh->m_CullFace))
							{
								Mesh::Meta meta = Serializer::DeserializeMeshMeta(mesh->GetFilePath());
								meta.m_CullFace = mesh->m_CullFace;
								Serializer::SerializeMeshMeta(meta);
							}

							int cullFaceModesIndices[3] = { GL_FRONT, GL_BACK, GL_FRONT_AND_BACK };
							int selectedCullFace = -1;
							for (size_t i = 0; i < 3; i++)
							{
								if (cullFaceModesIndices[i] == mesh->m_ShadowCullFace)
								{
									selectedCullFace = i;
								}
							}

							const char* cullFaceModes[] = { "Front", "Back", "None" };
							if (ImGui::Combo("Cull Face Mode", &selectedCullFace, cullFaceModes, 3))
							{
								Mesh::Meta meta = Serializer::DeserializeMeshMeta(mesh->GetFilePath());
								meta.m_ShadowCullFace = cullFaceModesIndices[selectedCullFace];
								mesh->m_ShadowCullFace = cullFaceModesIndices[selectedCullFace];
								Serializer::SerializeMeshMeta(meta);
							}
						}

						if (lod < r3d->m_LodsDistance.size())
						{
							ImGui::PushID(std::string("Distance" + std::to_string(lod)).c_str());
							ImGui::SliderFloat("Distance", &r3d->m_LodsDistance[lod],
								lod == 0 ? 0.0f : r3d->m_LodsDistance[lod - 1],
								lod + 1 == r3d->m_LodsDistance.size() ? 1000.0f : r3d->m_LodsDistance[lod + 1]);
							ImGui::PopID();
						}
					}
				};

				lodSection("LOD1", 0);
				lodSection("LOD2", 1);
				lodSection("LOD3", 2);

				if (ImGui::CollapsingHeader("Material"))
				{
					Indent indentMaterial;

					if (ImGui::BeginDragDropTarget())
					{
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSETS_BROWSER_ITEM"))
						{
							std::string path((const char*)payload->Data);
							path.resize(payload->DataSize);

							if (Utils::Contains(path, ".mat"))
							{
								r3d->SetMaterial(MaterialManager::GetInstance().Load(path));
							}
						}

						ImGui::EndDragDropTarget();
					}

					ImGui::Text("FilePath: %s", r3d->m_Material ? r3d->m_Material->GetFilePath().c_str() : "None");
					ImGui::Text("Name: %s", r3d->m_Material ? r3d->m_Material->GetName().c_str() : "None");

					if (ImGui::ImageButton((ImTextureID)r3d->m_Material->m_BaseColor->GetRendererID(), { 64.0f, 64.0f }, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f)))
					{
						m_TextureMenu.m_IsActive = true;
						m_TextureMenu.m_Texture = r3d->m_Material->m_BaseColor;
					}

					if (ImGui::BeginDragDropTarget())
					{
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSETS_BROWSER_ITEM"))
						{
							std::string path((const char*)payload->Data);
							path.resize(payload->DataSize);

							TextureManager::GetInstance().AsyncLoad(path,
								[=](Texture* texture) 
							{
								r3d->m_Material->m_BaseColor = texture;
							});
						}
						ImGui::EndDragDropTarget();
					}

					ImGui::SameLine();

					ImGui::PushID(imageOffset++);

					if (ImGui::Button("Reset"))
					{
						r3d->m_Material->m_BaseColor = TextureManager::GetInstance().White();
					}

					ImGui::PopID();

					if (ImGui::ImageButton((ImTextureID)r3d->m_Material->m_NormalMap->GetRendererID(), { 64.0f, 64.0f }, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f)))
					{
						m_TextureMenu.m_IsActive = true;
						m_TextureMenu.m_Texture = r3d->m_Material->m_NormalMap;
					}

					if (ImGui::BeginDragDropTarget())
					{
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSETS_BROWSER_ITEM"))
						{
							std::string path((const char*)payload->Data);
							path.resize(payload->DataSize);

							TextureManager::GetInstance().AsyncLoad(path,
								[=](Texture* texture)
							{
								r3d->m_Material->m_NormalMap = texture;
							});
						}
						ImGui::EndDragDropTarget();
					}

					ImGui::SameLine();

					ImGui::PushID(imageOffset++);

					ImGui::Checkbox("Use", &r3d->m_Material->m_UseNormalMap);

					ImGui::SameLine();

					if (ImGui::Button("Reset"))
					{
						r3d->m_Material->m_NormalMap = TextureManager::GetInstance().White();
					}

					ImGui::PopID();

					ImGui::ColorEdit3("Albedo", &r3d->m_Material->m_Albedo[0], ImGuiColorEditFlags_Float);
					ImGui::SliderFloat4("Uv Transform", &r3d->m_Material->m_UvTransform[0], 0.0f, 1.0f);
					ImGui::SliderFloat("Intensity", &r3d->m_Material->m_Intensity, 0.0f, 15.0f);
					ImGui::SliderFloat("Alpha", &r3d->m_Material->m_Alpha, 0.0f, 1.0f);

					if (r3d->m_Material->m_UseMetallicMap)
					{
						if (ImGui::ImageButton((ImTextureID)r3d->m_Material->m_MetallicMap->GetRendererID(), { 64.0f, 64.0f }, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f)))
						{
							m_TextureMenu.m_IsActive = true;
							m_TextureMenu.m_Texture = r3d->m_Material->m_MetallicMap;
						}

						if (ImGui::BeginDragDropTarget())
						{
							if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSETS_BROWSER_ITEM"))
							{
								std::string path((const char*)payload->Data);
								path.resize(payload->DataSize);

								TextureManager::GetInstance().AsyncLoad(path,
									[=](Texture* texture)
									{
										r3d->m_Material->m_MetallicMap = texture;
									});
							}
							ImGui::EndDragDropTarget();
						}

						ImGui::SameLine();

						ImGui::PushID(imageOffset++);

						if (ImGui::Button("Reset"))
						{
							r3d->m_Material->m_MetallicMap = TextureManager::GetInstance().White();
						}

						ImGui::SameLine();

						if (ImGui::Button("Switch"))
						{
							r3d->m_Material->m_UseMetallicMap = !r3d->m_Material->m_UseMetallicMap;
						}

						ImGui::PopID();
					}
					else
					{
						ImGui::SliderFloat("Metallic", &r3d->m_Material->m_Metallic, 0.0f, 1.0f);

						ImGui::SameLine();

						ImGui::PushID(imageOffset++);

						if (ImGui::Button("Switch"))
						{
							r3d->m_Material->m_UseMetallicMap = !r3d->m_Material->m_UseMetallicMap;
						}

						ImGui::PopID();
					}
					
					if (r3d->m_Material->m_UseRoughnessMap)
					{
						if (ImGui::ImageButton((ImTextureID)r3d->m_Material->m_RoughnessMap->GetRendererID(), { 64.0f, 64.0f }, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f)))
						{
							m_TextureMenu.m_IsActive = true;
							m_TextureMenu.m_Texture = r3d->m_Material->m_RoughnessMap;
						}

						if (ImGui::BeginDragDropTarget())
						{
							if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSETS_BROWSER_ITEM"))
							{
								std::string path((const char*)payload->Data);
								path.resize(payload->DataSize);

								TextureManager::GetInstance().AsyncLoad(path,
									[=](Texture* texture)
									{
										r3d->m_Material->m_RoughnessMap = texture;
									});
							}
							ImGui::EndDragDropTarget();
						}

						ImGui::SameLine();

						ImGui::PushID(imageOffset++);

						if (ImGui::Button("Reset"))
						{
							r3d->m_Material->m_RoughnessMap = TextureManager::GetInstance().White();
						}

						ImGui::PopID();

						ImGui::SameLine();

						ImGui::PushID(imageOffset++);

						if (ImGui::Button("Switch"))
						{
							r3d->m_Material->m_UseRoughnessMap = !r3d->m_Material->m_UseRoughnessMap;
						}

						ImGui::PopID();
					}
					else
					{
						ImGui::SliderFloat("Roughness", &r3d->m_Material->m_Roughness, 0.0f, 1.0f);

						ImGui::SameLine();

						ImGui::PushID(imageOffset++);

						if (ImGui::Button("Switch"))
						{
							r3d->m_Material->m_UseRoughnessMap = !r3d->m_Material->m_UseRoughnessMap;
						}

						ImGui::PopID();
					}

					if (ImGui::ImageButton((ImTextureID)r3d->m_Material->m_AoMap->GetRendererID(), { 64.0f, 64.0f }, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f)))
					{
						m_TextureMenu.m_IsActive = true;
						m_TextureMenu.m_Texture = r3d->m_Material->m_AoMap;
					}

					if (ImGui::BeginDragDropTarget())
					{
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSETS_BROWSER_ITEM"))
						{
							std::string path((const char*)payload->Data);
							path.resize(payload->DataSize);

							TextureManager::GetInstance().AsyncLoad(path,
								[=](Texture* texture)
								{
									r3d->m_Material->m_AoMap = texture;
								});
						}
						ImGui::EndDragDropTarget();
					}

					ImGui::SameLine();

					ImGui::PushID(imageOffset++);

					if (ImGui::Button("Reset"))
					{
						r3d->m_Material->m_AoMap = TextureManager::GetInstance().White();
					}

					ImGui::PopID();

					for (auto& [name, value] : r3d->m_Material->m_BaseMaterial->m_FloatUniformsByName)
					{
						float newValue = value;
						if (ImGui::SliderFloat(name.c_str(), &newValue, 0.0f, 1.0f))
						{
							value = newValue;
						}
					}

					for (auto& [name, value] : r3d->m_Material->m_BaseMaterial->m_IntUniformsByName)
					{
						int newValue = value;
						if (ImGui::InputInt(name.c_str(), &newValue))
						{
							value = newValue;
						}
					}

					for (auto& [name, value] : r3d->m_Material->m_BaseMaterial->m_Vec2UniformsByName)
					{
						DrawVec2Control(name, value);
					}

					for (auto& [name, value] : r3d->m_Material->m_BaseMaterial->m_Vec3UniformsByName)
					{
						DrawVec3Control(name, value);
					}

					for (auto& [name, value] : r3d->m_Material->m_BaseMaterial->m_Vec4UniformsByName)
					{
						DrawVec4Control(name, value);
					}

					for (auto& [name, value] : r3d->m_Material->m_BaseMaterial->m_TextureUniformsByName)
					{
						if (ImGui::ImageButton((ImTextureID)value->GetRendererID(), { 64.0f, 64.0f }, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f)))
						{
							m_TextureMenu.m_IsActive = true;
							m_TextureMenu.m_Texture = value;
						}

						if (ImGui::BeginDragDropTarget())
						{
							if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSETS_BROWSER_ITEM"))
							{
								std::string path((const char*)payload->Data);
								path.resize(payload->DataSize);

								TextureManager::GetInstance().AsyncLoad(path,
									[=, _name = name](Texture* texture)
									{
										r3d->m_Material->m_BaseMaterial->m_TextureUniformsByName[_name] = texture;
									});
							}
							ImGui::EndDragDropTarget();
						}

						ImGui::SameLine();

						ImGui::PushID(value->GetRendererID() + imageOffset++);

						if (ImGui::Button("Reset"))
						{
							r3d->m_Material->m_BaseMaterial->m_TextureUniformsByName[name] = TextureManager::GetInstance().White();
						}

						ImGui::PopID();

						ImGui::SameLine();

						ImGui::Text(name.c_str());
					}

					if (ImGui::Button("Save"))
					{
						Serializer::SerializeMaterial(r3d->m_Material->GetFilePath(), r3d->m_Material);
					}
				}

				ImGui::Checkbox("Is Opaque", &r3d->m_IsOpaque);
				ImGui::Checkbox("Draw Shadows", &r3d->m_DrawShadows);
			}
		}
	}
}

void Editor::UserDefinedComponents(GameObject* gameObject)
{
	if (gameObject->m_ComponentManager.GetComponents().size() > 0) ImGui::Text("User-defined components");
	for (IComponent* component : gameObject->m_ComponentManager.GetComponents())
	{
		if (Utils::IsUserDefinedComponent(component->GetType()) && !component->GetType().size() == 0)
		{
			if (RemoveComponentMenu(gameObject, component))
			{
				if (ImGui::CollapsingHeader(component->GetType().c_str()))
				{
					Indent indent;

					std::function<void(void*, const std::string&, size_t)> show = [&show, this](void* component, const std::string& typeName, size_t offset)
					{
						ReflectionSystem& reflectionSystem = ReflectionSystem::GetInstance();
						auto classByType = reflectionSystem.m_ClassesByType.find(typeName);
						if (classByType == reflectionSystem.m_ClassesByType.end())
						{
							return;
						}

						const std::string componentType = typeName;

						for (auto& [name, prop] : classByType->second.m_PropertiesByName)
						{
							if (prop.IsValue<bool>())
							{
								bool value = reflectionSystem.GetValue<bool>(component, componentType, name);
								ImGui::PushID(std::string(componentType + name).c_str());
								if (ImGui::Checkbox(name.c_str(), &value))
								{
									reflectionSystem.SetValue(value, component, componentType, name);
								}
								ImGui::PopID();
							}
							else if (prop.IsValue<float>())
							{
								float value = reflectionSystem.GetValue<float>(component, componentType, name);
								ImGui::PushID(std::string(componentType + name).c_str());
								if (ImGui::SliderFloat(name.c_str(), &value, 0.0f, 10.0f))
								{
									reflectionSystem.SetValue(value, component, componentType, name);
								}
								ImGui::PopID();
							}
							else if (prop.IsValue<int>())
							{
								int value = reflectionSystem.GetValue<int>(component, componentType, name);
								ImGui::PushID(std::string(componentType + name).c_str());
								if (ImGui::SliderInt(name.c_str(), &value, 0, 10))
								{
									reflectionSystem.SetValue(value, component, componentType, name);
								}
								ImGui::PopID();
							}
							else if (prop.IsValue<double>())
							{
								double value = reflectionSystem.GetValue<double>(component, componentType, name);
								ImGui::PushID(std::string(componentType + name).c_str());
								if (ImGui::InputDouble(name.c_str(), &value))
								{
									reflectionSystem.SetValue(value, component, componentType, name);
								}
								ImGui::PopID();
							}
							else if (prop.IsValue<std::string>())
							{
								std::string value = reflectionSystem.GetValue<std::string>(component, componentType, name);
								char buffer[64];
								strcpy(buffer, value.c_str());
								ImGui::PushID(std::string(componentType + name).c_str());
								if (ImGui::InputText(name.c_str(), buffer, sizeof(buffer)))
								{
									reflectionSystem.SetValue(std::string(buffer), component, componentType, name);
								}

								if (ImGui::BeginDragDropTarget())
								{
									if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSETS_BROWSER_ITEM"))
									{
										std::string path((const char*)payload->Data);
										path.resize(payload->DataSize);
										reflectionSystem.SetValue(path, component, componentType, name);
									}
									else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GAMEOBJECT"))
									{
										reflectionSystem.SetValue(m_CurrentScene->FindGameObjectByUUID(m_SelectedGameObjects[0])->GetName(), component, componentType, name);
									}
									ImGui::EndDragDropTarget();
								}

								ImGui::PopID();
							}
							else if (prop.IsValue<GameObject*>())
							{
								GameObject* value = reflectionSystem.GetValue<GameObject*>(component, componentType, name);
								const ImTextureID gameObjectIconId = (ImTextureID)TextureManager::GetInstance().GetByName("FileIcon")->GetRendererID();
								if (ImGui::ImageButton(gameObjectIconId, { 16, 16 }, ImVec2(0, 1), ImVec2(1, 0)))
								{
									reflectionSystem.SetValue<GameObject*>(nullptr, component, componentType, name);
								}

								if (ImGui::BeginDragDropTarget())
								{
									if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GAMEOBJECT"))
									{
										std::string uuid((const char*)payload->Data);
										uuid.resize(payload->DataSize);
										reflectionSystem.SetValue(m_CurrentScene->FindGameObjectByUUID(uuid), component, componentType, name);
									}
									ImGui::EndDragDropTarget();
								}

								ImGui::SameLine();
								ImGui::Text(std::string((value ? value->GetName() : "Null")).c_str());
							}
							else if (prop.IsValue<glm::vec2>())
							{
								glm::vec2 value = reflectionSystem.GetValue<glm::vec2>(component, componentType, name);
								DrawVec2Control(name.c_str(), value);
								reflectionSystem.SetValue(value, component, componentType, name);
							}
							else if (prop.IsValue<glm::vec3>())
							{
								glm::vec3 value = reflectionSystem.GetValue<glm::vec3>(component, componentType, name);
								DrawVec3Control(name.c_str(), value);
								reflectionSystem.SetValue(value, component, componentType, name);
							}
							else if (prop.IsValue<glm::vec4>())
							{
								glm::vec4 value = reflectionSystem.GetValue<glm::vec4>(component, componentType, name);
								DrawVec4Control(name.c_str(), value);
								reflectionSystem.SetValue(value, component, componentType, name);
							}
							else if (prop.IsValue<glm::ivec2>())
							{
								glm::ivec2 value = reflectionSystem.GetValue<glm::ivec2>(component, componentType, name);
								DrawIVec2Control(name.c_str(), value);
								reflectionSystem.SetValue(value, component, componentType, name);
							}
							else if (prop.IsValue<glm::ivec3>())
							{
								glm::ivec3 value = reflectionSystem.GetValue<glm::ivec3>(component, componentType, name);
								DrawIVec3Control(name.c_str(), value);
								reflectionSystem.SetValue(value, component, componentType, name);
							}
							else if (prop.IsValue<glm::ivec4>())
							{
								glm::ivec4 value = reflectionSystem.GetValue<glm::ivec4>(component, componentType, name);
								DrawIVec4Control(name.c_str(), value);
								reflectionSystem.SetValue(value, component, componentType, name);
							}

#define ADD_ELEMENT(_name, _value, _i) \
						ImGui::Text(_name.c_str()); \
						ImGui::SameLine(); \
						ImGui::PushID(std::string(_name + "+" + std::to_string(_i)).c_str()); \
						if (ImGui::Button("+")) { _value.emplace_back(); } ImGui::PopID();

#define REMOVE_ELEMENT(_name, _value, _i) \
						ImGui::SameLine(); \
						ImGui::PushID(std::string(_name + "-" + std::to_string(_i)).c_str()); \
						if (ImGui::Button("-")) { _value.erase(_value.begin() + _i); } ImGui::PopID();

							else if (prop.IsValue<std::vector<float>>())
						{
							std::vector<float> value = reflectionSystem.GetValue<std::vector<float>>(component, componentType, name);

							if (value.empty())
							{
								ADD_ELEMENT(name, value, -1)
							}

							for (size_t i = 0; i < value.size(); i++)
							{
								ImGui::InputFloat(std::string("##float" + std::to_string(i)).c_str(), &value[i]);

								REMOVE_ELEMENT(name, value, i)

									if (i == 0)
									{
										ImGui::SameLine();
										ADD_ELEMENT(name, value, i)
									}
							}

							reflectionSystem.SetValue(value, component, componentType, name);
						}
							else if (prop.IsValue<std::vector<int>>())
						{
							std::vector<int> value = reflectionSystem.GetValue<std::vector<int>>(component, componentType, name);

							if (value.empty())
							{
								ADD_ELEMENT(name, value, -1)
							}

							for (size_t i = 0; i < value.size(); i++)
							{
								ImGui::InputInt(std::string("##int" + name + std::to_string(i)).c_str(), &value[i]);

								REMOVE_ELEMENT(name, value, i)

									if (i == 0)
									{
										ImGui::SameLine();
										ADD_ELEMENT(name, value, i)
									}
							}

							reflectionSystem.SetValue(value, component, componentType, name);
						}
							else if (prop.IsValue<std::vector<double>>())
						{
							std::vector<double> value = reflectionSystem.GetValue<std::vector<double>>(component, componentType, name);

							if (value.empty())
							{
								ADD_ELEMENT(name, value, -1)
							}

							for (size_t i = 0; i < value.size(); i++)
							{
								ImGui::InputDouble(std::string("##double" + name + std::to_string(i)).c_str(), &value[i]);

								REMOVE_ELEMENT(name, value, i)

									if (i == 0)
									{
										ImGui::SameLine();
										ADD_ELEMENT(name, value, i)
									}
							}

							reflectionSystem.SetValue(value, component, componentType, name);
						}
							else if (prop.IsValue<std::vector<bool>>())
						{
							std::vector<bool> value = reflectionSystem.GetValue<std::vector<bool>>(component, componentType, name);

							if (value.empty())
							{
								ADD_ELEMENT(name, value, -1)
							}

							for (size_t i = 0; i < value.size(); i++)
							{
								bool valueBool = value[i];
								ImGui::Checkbox(std::string("##bool" + name + std::to_string(i)).c_str(), &valueBool);
								value[i] = valueBool;

								REMOVE_ELEMENT(name, value, i)

									if (i == 0)
									{
										ImGui::SameLine();
										ADD_ELEMENT(name, value, i)
									}
							}

							reflectionSystem.SetValue(value, component, componentType, name);
						}
							else if (prop.IsValue<std::vector<std::string>>())
						{
							std::vector<std::string> value = reflectionSystem.GetValue<std::vector<std::string>>(component, componentType, name);

							if (value.empty())
							{
								ADD_ELEMENT(name, value, -1)
							}

							for (size_t i = 0; i < value.size(); i++)
							{
								char buffer[64];
								strcpy(buffer, value[i].c_str());
								if (ImGui::InputText(std::string("##string" + name + std::to_string(i)).c_str(), buffer, sizeof(buffer)))
								{
									value[i] = buffer;
								}

								REMOVE_ELEMENT(name, value, i)

									if (i == 0)
									{
										ImGui::SameLine();
										ADD_ELEMENT(name, value, i)
									}
							}

							reflectionSystem.SetValue(value, component, componentType, name);
						}

							m_DrawVecLabel = false;

							if (prop.IsValue<std::vector<glm::vec2>>())
							{
								std::vector<glm::vec2> value = reflectionSystem.GetValue<std::vector<glm::vec2>>(component, componentType, name);

								if (value.empty())
								{
									ADD_ELEMENT(name, value, -1)
								}

								for (size_t i = 0; i < value.size(); i++)
								{
									if (i == 0) { ADD_ELEMENT(name, value, i); ImGui::SameLine(); }

									DrawVec2Control(std::string("vec2" + name + std::to_string(i)), value[i]);

									REMOVE_ELEMENT(name, value, i)
								}

								reflectionSystem.SetValue(value, component, componentType, name);
							}
							else if (prop.IsValue<std::vector<glm::vec3>>())
							{
								std::vector<glm::vec3> value = reflectionSystem.GetValue<std::vector<glm::vec3>>(component, componentType, name);

								if (value.empty())
								{
									ADD_ELEMENT(name, value, -1)
								}

								for (size_t i = 0; i < value.size(); i++)
								{
									if (i == 0) { ADD_ELEMENT(name, value, i); ImGui::SameLine(); }

									DrawVec3Control(std::string("vec3" + name + std::to_string(i)), value[i]);

									REMOVE_ELEMENT(name, value, i)
								}

								reflectionSystem.SetValue(value, component, componentType, name);
							}
							else if (prop.IsValue<std::vector<glm::vec4>>())
							{
								std::vector<glm::vec4> value = reflectionSystem.GetValue<std::vector<glm::vec4>>(component, componentType, name);

								if (value.empty())
								{
									ADD_ELEMENT(name, value, -1)
								}

								for (size_t i = 0; i < value.size(); i++)
								{
									if (i == 0) { ADD_ELEMENT(name, value, i); ImGui::SameLine(); }

									DrawVec4Control(std::string("vec4" + name + std::to_string(i)), value[i]);

									REMOVE_ELEMENT(name, value, i)
								}

								reflectionSystem.SetValue(value, component, componentType, name);
							}
							else if (prop.IsValue<std::vector<glm::ivec2>>())
							{
								std::vector<glm::ivec2> value = reflectionSystem.GetValue<std::vector<glm::ivec2>>(component, componentType, name);

								if (value.empty())
								{
									ADD_ELEMENT(name, value, -1)
								}

								for (size_t i = 0; i < value.size(); i++)
								{
									if (i == 0) { ADD_ELEMENT(name, value, i); ImGui::SameLine(); }

									DrawIVec2Control(std::string("ivec2" + name + std::to_string(i)), value[i]);

									REMOVE_ELEMENT(name, value, i)
								}

								reflectionSystem.SetValue(value, component, componentType, name);
							}
							else if (prop.IsValue<std::vector<glm::ivec3>>())
							{
								std::vector<glm::ivec3> value = reflectionSystem.GetValue<std::vector<glm::ivec3>>(component, componentType, name);

								if (value.empty())
								{
									ADD_ELEMENT(name, value, -1)
								}

								for (size_t i = 0; i < value.size(); i++)
								{
									if (i == 0) { ADD_ELEMENT(name, value, i); ImGui::SameLine(); }

									DrawIVec3Control(std::string("ivec3" + name + std::to_string(i)), value[i]);

									REMOVE_ELEMENT(name, value, i)
								}

								reflectionSystem.SetValue(value, component, componentType, name);
							}
							else if (prop.IsValue<std::vector<glm::ivec4>>())
							{
								std::vector<glm::ivec4> value = reflectionSystem.GetValue<std::vector<glm::ivec4>>(component, componentType, name);

								if (value.empty())
								{
									ADD_ELEMENT(name, value, -1)
								}

								for (size_t i = 0; i < value.size(); i++)
								{
									if (i == 0) { ADD_ELEMENT(name, value, i); ImGui::SameLine(); }

									DrawIVec4Control(std::string("ivec4" + name + std::to_string(i)), value[i]);

									REMOVE_ELEMENT(name, value, i)
								}

								reflectionSystem.SetValue(value, component, componentType, name);
							}

							m_DrawVecLabel = true;
						}

						for (const auto& parent : classByType->second.m_Parents)
						{
							show(component, parent.first, parent.second);
						}
					};

					show(component, component->GetType(), 0);
				}
			}
		}
	}
}

void Editor::SplineComponent(GameObject* gameObject)
{
	Spline* spline = gameObject->m_ComponentManager.GetComponent<Spline>();
	if (spline)
	{
		if (RemoveComponentMenu(gameObject, spline))
		{
			if (ImGui::CollapsingHeader("Spline"))
			{
				Indent indent;

				ImGui::SliderFloat("Speed", &spline->m_Speed, 0.0f, 5.0f);
				ImGui::Checkbox("Visualize", &spline->m_Visualize);
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
		else if (ImGui::MenuItem("Delete selected"))
		{
			for (const std::string& gameObjectUUID : m_SelectedGameObjects)
			{
				if (GameObject* gameObject = m_CurrentScene->FindGameObjectByUUID(gameObjectUUID))
				{
					m_CurrentScene->DeleteGameObject(gameObject);
				}
			}
			m_SelectedGameObjects.clear();
		}
		ImGui::EndPopup();
	}
}

void Editor::TextMenu()
{
	Gui& gui = Gui::GetInstance();
	if (ImGui::Begin("UI text"))
	{
		ImGui::SliderFloat("Width", &gui.m_TextProps.m_Width, 0.0f, 1.0f);
		ImGui::SliderFloat("Border edge", &gui.m_TextProps.m_BorderEdge, 0.0f, 1.0f);
		ImGui::SliderFloat("Border width", &gui.m_TextProps.m_BorderWidth, 0.0f, 2.0f);
		ImGui::SliderFloat("Edge", &gui.m_TextProps.m_Edge, 0.0f, 1.0f);
		ImGui::SliderFloat2("Shadow offset", &gui.m_TextProps.m_ShadowOffset[0], -0.015f, 0.015f);
		ImGui::ColorEdit4("Color", &gui.m_TextProps.m_OutLineColor[0]);
		ImGui::End();
	}
}

void Editor::TextureMenu()
{
	if (m_TextureMenu.m_IsActive && ImGui::Begin("Texture Menu", &m_TextureMenu.m_IsActive, ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse))
	{
		if (m_TextureMenu.m_Texture)
		{
			ImGui::Image((void*)m_TextureMenu.m_Texture->GetRendererID(), { 128.0f, 128.0f });
			ImGui::Text("Name: %s", m_TextureMenu.m_Texture->GetName().c_str());
			ImGui::Text("FilePath: %s", m_TextureMenu.m_Texture->GetFilePath().c_str());
			ImGui::Text("Size: %.0f %.0f", m_TextureMenu.m_Texture->GetSize().x, m_TextureMenu.m_Texture->GetSize().y);
			if (ImGui::Button("Linear"))
			{
				std::string name = m_TextureMenu.m_Texture->GetName();
				TextureManager::GetInstance().m_TexParameters[0] = { GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR };
				TextureManager::GetInstance().m_TexParameters[1] = { GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR };
				m_TextureMenu.m_Texture->Reload(TextureManager::GetInstance().m_TexParameters, { 0, 1 });
				TextureManager::GetInstance().ResetTexParametersi();
				Serializer::SerializeTextureMeta(m_TextureMenu.m_Texture->m_Meta);
			}
			if (ImGui::Button("Repeat"))
			{
				std::string name = m_TextureMenu.m_Texture->GetName();
				TextureManager::GetInstance().m_TexParameters[2] = { GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT };
				TextureManager::GetInstance().m_TexParameters[3] = { GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT };
				m_TextureMenu.m_Texture->Reload(TextureManager::GetInstance().m_TexParameters, { 2, 3 });
				TextureManager::GetInstance().ResetTexParametersi();
				Serializer::SerializeTextureMeta(m_TextureMenu.m_Texture->m_Meta);
			}
			if (ImGui::Button("Clamp"))
			{
				std::string name = m_TextureMenu.m_Texture->GetName();
				TextureManager::GetInstance().m_TexParameters[2] = { GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE };
				TextureManager::GetInstance().m_TexParameters[3] = { GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE };
				m_TextureMenu.m_Texture->Reload(TextureManager::GetInstance().m_TexParameters, { 2, 3 });
				TextureManager::GetInstance().ResetTexParametersi();
				Serializer::SerializeTextureMeta(m_TextureMenu.m_Texture->m_Meta);
			}
		}
		ImGui::End();
	}
}

void Editor::DeleteDirectoryOrFile::Update()
{
	ImGui::SetNextWindowSize({ 250.0f, 60.0f });
	if (m_IsActive && ImGui::Begin("Deleting directory or file", &m_IsActive,
		ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar))
	{
		ImGui::Text("Are you sure you want to delete\n%s?", m_FileName.c_str());
		if (ImGui::Button("Yes"))
		{
			if (Utils::GetResolutionFromFilePath(m_FilePath.string()) == "None")
			{
				Utils::DeleteDirectory(m_FilePath.wstring(), true);
			}
			{
				std::filesystem::remove(m_FilePath);
			}

			m_FilePath.clear();
			m_IsActive = false;
		}
		ImGui::SameLine();
		if(ImGui::Button("No"))
		{
			m_FilePath.clear();
			m_IsActive = false;
		}
		ImGui::End();
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
		if (ImGui::MenuItem("Box Collider2D"))
		{
			gameObject->m_ComponentManager.AddComponent<BoxCollider2D>();
		}
		if (ImGui::MenuItem("Box Collider3D"))
		{
			gameObject->m_ComponentManager.AddComponent<BoxCollider3D>();
		}
		if (ImGui::MenuItem("Circle Collider2D"))
		{
			gameObject->m_ComponentManager.AddComponent<CircleCollider2D>();
		}
		if (ImGui::MenuItem("Sphere Collider3D"))
		{
			gameObject->m_ComponentManager.AddComponent<SphereCollider3D>();
		}
		if (ImGui::MenuItem("Rigidbody2D"))
		{
			gameObject->m_ComponentManager.AddComponent<Rigidbody2D>();
		}
		if (ImGui::MenuItem("Rigidbody3D"))
		{
			gameObject->m_ComponentManager.AddComponent<Rigidbody3D>();
		}
		if (ImGui::MenuItem("Animator 2D"))
		{
			gameObject->m_ComponentManager.AddComponent<Animator2D>();
		}
		if (ImGui::MenuItem("Particle emitter"))
		{
			gameObject->m_ComponentManager.AddComponent<ParticleEmitter>();
		}
		if (ImGui::MenuItem("Script"))
		{
			gameObject->m_ComponentManager.AddComponent<Script>();
		}
		if (ImGui::MenuItem("Point Light"))
		{
			gameObject->m_ComponentManager.AddComponent<PointLight>();
		}
		if (ImGui::MenuItem("Spot Light"))
		{
			gameObject->m_ComponentManager.AddComponent<SpotLight>();
		}
		if (ImGui::MenuItem("Directional Light"))
		{
			gameObject->m_ComponentManager.AddComponent<DirectionalLight>();
		}
		if (ImGui::MenuItem("Renderer3D"))
		{
			gameObject->m_ComponentManager.AddComponent<Renderer3D>();
		}
		if (ImGui::MenuItem("Spline"))
		{
			gameObject->m_ComponentManager.AddComponent<Spline>();
		}

		for (auto& classesByType : ReflectionSystem::GetInstance().m_ClassesByType)
		{
			if (Utils::IsUserDefinedComponent(classesByType.first) && ImGui::MenuItem(classesByType.first.c_str()))
			{
				classesByType.second.m_AddComponentCallBack(&gameObject->m_ComponentManager);
			}
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
	if (Window::GetInstance().GetSize().x == 0 || Window::GetInstance().GetSize().y == 0) return;

	Timer timer = Timer(false, &m_Stats.m_RenderEditor);

	m_Stats.s_AllocationsCount = 0;

	m_CurrentScene = scene;

	static bool dockspaceOpen = true;
	static bool optFullscreenPersistant = true;
	bool optFullscreen = optFullscreenPersistant;
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking;

#ifdef STANDALONE
	dockspace_flags |= ImGuiDockNodeFlags_NoTabBar;
#else
	windowFlags |= ImGuiWindowFlags_MenuBar;
#endif

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

#ifndef STANDALONE
	if (m_ShowNavigation)
	{
		Navigation();
	}
	ShortCuts();
#endif

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.0f, 0.0f });

	Viewport& viewport = Viewport::GetInstance();
	viewport.Update();

	EntryPoint::GetApplication().OnImGuiRender();

	if (!m_IsEnabled)
	{
		ImGui::PopStyleVar();

		ImGui::End();
		return;
	}

	Debug();
	Properties();
	AssetBrowser();
	Hierarchy();
	AssetList();
	TextureMenu();
	TextMenu();
	ToolBar();
	Environment();
	Settings();
	m_Animation2DMenu.Update();
	m_TextureAtlasMenu.Update();
	m_DeleteDirectoryOrFile.Update();
	m_CreatingFileMenu.Update();

	if (Input::Mouse::IsMousePressed(Keycode::MOUSE_BUTTON_1) && viewport.IsFocused() && viewport.IsHovered()
		&& !viewport.IsActiveGuizmo())
	{
		std::vector<GameObject*> ignoreMask;
		int maxGameObjects = 1;

		if (!Input::KeyBoard::IsKeyDown(Keycode::KEY_LEFT_CONTROL))
		{
			for (const std::string& gameObjectUUID : m_SelectedGameObjects)
			{
				if (GameObject* gameObject = m_CurrentScene->FindGameObjectByUUID(gameObjectUUID))
				{
					ignoreMask.push_back(gameObject);
				}
			}

			m_SelectedGameObjects.clear();
			std::vector<GameObject*> selectedGameObjects = m_CurrentScene->SelectGameObject(ignoreMask, maxGameObjects);
			if (!selectedGameObjects.empty())
			{
				for (GameObject* gameObject : selectedGameObjects)
				{
					if (!Utils::Erase<std::string>(m_SelectedGameObjects, gameObject->GetUUID()))
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
					if (Input::KeyBoard::IsKeyDown(Keycode::KEY_X))
					{
						Utils::Erase<std::string>(m_SelectedGameObjects, gameObject->GetUUID());
					}
					else if (selectedGameObjects.size() > 1 && !Utils::IsThere<std::string>(m_SelectedGameObjects, gameObject->GetUUID()))
					{
						AddSelectedGameObject(gameObject);
					}
					else if (selectedGameObjects.size() == 1 && !Utils::Erase<std::string>(m_SelectedGameObjects, gameObject->GetUUID()))
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
			Window::GetInstance().Exit();
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
			m_Animation2DMenu.m_IsActive = true;
		}
		else if (ImGui::MenuItem("Texture Atlas"))
		{
			m_TextureAtlasMenu.m_IsActive = true;
		}
		else if (ImGui::MenuItem("Asset List"))
		{
			m_IsActiveAssetList = true;
		}
		ImGui::EndMenu();
	}
}

void Editor::Settings()
{
	if (ImGui::Begin("Settings"))
	{
		ImGui::Checkbox("Polygon mode", &m_PolygonMode);
		ImGui::Checkbox("Draw Bounding Boxes", &m_DrawBoundingBoxes);
		ImGui::Checkbox("Draw Lights", &m_DrawLights);
		ImGui::Checkbox("Snap", &m_Snap);
		ImGui::Checkbox("Draw colliders", &m_DrawColliders);
		ImGui::SliderInt("Line width", &m_LineWidth, 1, 5);
		ImGui::SliderFloat("Thumbnail scale", &m_ThumbnailScale, 0.1f, 5.0f);

		if (ImGui::SliderFloat("Master volume", &SoundManager::GetInstance().m_MasterVolume, 0.0f, 1.0f))
		{
			SoundManager::GetInstance().UpdateVolume();
		}

		if (ImGui::CollapsingHeader("Outline"))
		{
			ImGui::PushID("Outline");
			ImGui::ColorEdit3("Color", &m_OutlineParams.m_Color[0]);
			ImGui::SliderInt("Color", &m_OutlineParams.m_Thickness, 1, 5);
			ImGui::PopID();
		}

		if (ImGui::Button("Reload shaders"))
		{
			Shader::ReloadAll();
		}

		if (ImGui::Button("Save materials"))
		{
			auto materials = MaterialManager::GetInstance().GetMaterials();
			for (const auto& [filePath, material] : materials)
			{
				Serializer::SerializeMaterial(filePath, material);
			}
		}

		ImGui::End();
	}
}

void Editor::AddSelectedGameObject(GameObject* gameObject)
{
	if (!Utils::IsThere<std::string>(m_SelectedGameObjects, gameObject->GetUUID()))
	{
		m_SelectedGameObjects.push_back(gameObject->GetUUID());
	}
}

void Editor::ShortCuts()
{
	if (Input::KeyBoard::IsKeyDown(Keycode::KEY_LEFT_CONTROL))
	{
		if (Input::KeyBoard::IsKeyPressed(Keycode::KEY_D))
		{
			std::vector<GameObject*> newGameObjects;
			for (const std::string& gameObjectUUID : m_SelectedGameObjects)
			{
				if (GameObject* gameObject = m_CurrentScene->FindGameObjectByUUID(gameObjectUUID))
				{
					GameObject* newGameObject = m_CurrentScene->CreateGameObject(gameObject->GetName());
					if (GameObject* owner = gameObject->GetOwner())
					{
						owner->AddChild(newGameObject);
					}
					newGameObject->Copy(*gameObject);
					newGameObjects.push_back(newGameObject);
				}
			}

			if (!m_SelectedGameObjects.empty())
			{
				for (GameObject* gameObject : newGameObjects)
				{
					m_SelectedGameObjects.clear();
					m_SelectedGameObjects.emplace_back(gameObject->GetUUID());
				}
			}
		}

		if (Input::KeyBoard::IsKeyPressed(Keycode::KEY_A) && Viewport::GetInstance().IsFocused())
		{
			m_SelectedGameObjects.clear();
			for (GameObject* gameObject : m_CurrentScene->GetGameObjects())
			{
				m_SelectedGameObjects.emplace_back(gameObject->GetUUID());
			}
		}
	}

	if (Input::KeyBoard::IsKeyPressed(Keycode::KEY_DELETE))
	{
		for (const std::string& gameObjectUUID : m_SelectedGameObjects)
		{
			if (GameObject* gameObject = m_CurrentScene->FindGameObjectByUUID(gameObjectUUID))
			{
				m_CurrentScene->DeleteGameObject(gameObject);
			}
		}
		m_SelectedGameObjects.clear();
	}

	if (Input::KeyBoard::IsKeyPressed(Keycode::KEY_F) && Viewport::GetInstance().IsFocused())
	{
		m_IsEnabled = !m_IsEnabled;
	}
}

void Editor::CreatingFileMenu::Update()
{
	ImGui::SetNextWindowSize({ 350.0f, 80.0f });
	if (m_IsActive && ImGui::Begin("Create file", &m_IsActive,
		ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar))
	{
		ImGui::Text("FilePath: %s", m_FilePath.string().c_str());
		ImGui::InputText("Name", m_FileName, sizeof(m_CreatingFileMenu.m_FileName));

		if (ImGui::Button("Create"))
		{
			if (m_FileName[0] != '\0' && !m_FilePath.empty())
			{
				std::string filePath = std::string(m_FilePath.string() + "/" + m_FileName);
				if (Utils::GetResolutionFromFilePath(m_FileName) == "None")
				{
					std::filesystem::create_directory(filePath);
				}
				else
				{
					std::ofstream fout(filePath);
				}

				m_FilePath.clear();
				m_FileName[0] = '\0';
				m_IsActive = false;
			}
		}

		ImGui::SameLine();

		if (ImGui::Button("Cancel"))
		{
			m_FilePath.clear();
			m_FileName[0] = '\0';
			m_IsActive = false;
		}

		ImGui::End();
	}
}

bool Editor::RemoveComponentMenu(GameObject* gameObject, IComponent* component)
{
	ImGui::PushID(component);
	if (ImGui::Button("X"))
	{
		gameObject->m_ComponentManager.RemoveComponent(component);
		ImGui::PopID();
		return false;
	}
	ImGui::PopID();
	ImGui::SameLine();
	return true;
}

void Editor::DropMaterialOnViewport(const std::string& filePath)
{
	std::vector<GameObject*> selectedGameObjects = m_CurrentScene->SelectGameObject({}, 1);
	if (selectedGameObjects.size() == 1)
	{
		GameObject* selectedGameObject = selectedGameObjects[0];
		if (Renderer3D* r3d = selectedGameObject->m_ComponentManager.GetComponent<Renderer3D>())
		{
			r3d->SetMaterial(MaterialManager::GetInstance().Load(filePath));
		}
	}
}

void Editor::ResetStats()
{
	m_Stats = Stats();
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

void Editor::Animation2DMenu::Update()
{
	if (m_IsActive && ImGui::Begin("Animation 2D", &m_IsActive, ImGuiWindowFlags_NoCollapse))
	{
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSETS_BROWSER_ITEM"))
			{
				std::string path((const char*)payload->Data);
				path.resize(payload->DataSize);
				Texture* texture = TextureManager::GetInstance().GetByFilePath(path);
				if (m_Animation)
				{
					if (texture)
					{
						m_Animation->m_Textures.push_back(texture);
						m_Animation->m_UVs.push_back(Utils::StandartUV());
					}
					else if (std::filesystem::is_directory(path))
					{
						std::vector<Texture*> textures = TextureManager::GetInstance().GetTexturesFromFolder(path);
						size_t texturesSize = textures.size();
						for (size_t i = 0; i < texturesSize; i++)
						{
							m_Animation->m_Textures.push_back(textures[i]);
							m_Animation->m_UVs.push_back(Utils::StandartUV());
						}
					}
				}
				if (Utils::Contains(path, ".anim"))
				{
					m_Animation = Animation2DManager::GetInstance().Load(path);
				}
			}
			else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TEXTURE_ATLAS"))
			{
				TextureAtlasData* data = (TextureAtlasData*)payload->Data;
				if (data)
				{
					m_Animation->m_Textures.push_back(data->m_TextureAtlas->GetTexture());
					m_Animation->m_UVs.push_back(data->m_TextureAtlas->GetUV({ data->m_J, data->m_I }));
				}
			}

			ImGui::EndDragDropTarget();
		}

		if (!m_Animation)
		{
			ImGui::End();
			return;
		}
		else
		{
			ImGui::Text("Filepath: %s", m_Animation->GetFilePath().c_str());
		}

		const float padding = 16.0f;
		const float thumbnailSize = 128.0f;
		const float cellSize = padding + thumbnailSize;
		const float panelWidth = ImGui::GetContentRegionAvail().x;

		const int columns = (int)(panelWidth / cellSize);
		ImGui::Columns(glm::max<int>(columns, 1), 0, false);

		for (size_t i = 0; i < m_Animation->m_Textures.size(); i++)
		{
			Texture* texture = m_Animation->m_Textures[i];
			std::vector<float> uv = m_Animation->m_UVs[i];

			if (!texture) continue;

			ImGui::Columns(glm::max<int>(columns, 1), 0, false);
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
			ImGui::PushID(texture->GetFilePath().c_str());
			if (ImGui::ImageButton((ImTextureID)texture->GetRendererID(), { thumbnailSize , thumbnailSize }, ImVec2(uv[0], uv[5]), ImVec2(uv[4], uv[1])))
			{
				m_Animation->m_Textures.erase(
					m_Animation->m_Textures.begin() + i);
				m_Animation->m_UVs.erase(
					m_Animation->m_UVs.begin() + i);
			}
			ImGui::PopID();
			ImGui::PopStyleColor();
			ImGui::NextColumn();
		}
		ImGui::Columns(1);

		if (ImGui::Button("Save"))
		{
			Animation2DManager::GetInstance().Save(m_Animation);
			Serializer::SerializeAnimation2D(m_Animation);
		}

		ImGui::SameLine();

		if (ImGui::Button("Clear"))
		{
			m_Animation->m_Textures.clear();
			m_Animation->m_UVs.clear();
		}

		ImGui::Text("Loaded animations");
		for (auto& anim : Animation2DManager::GetInstance().m_Animations)
		{
			ImGui::Text("%s", anim.first.c_str());
		}

		ImGui::End();
	}
}

void Editor::TextureAtlasMenu::Update()
{
	if (m_IsActive && ImGui::Begin("Texture atlas", &m_IsActive, ImGuiWindowFlags_NoCollapse))
	{
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSETS_BROWSER_ITEM"))
			{
				std::string path((const char*)payload->Data);
				path.resize(payload->DataSize);
				if (Utils::Contains(path, ".ta"))
				{
					m_TextureAtlas = Serializer::DeSerializeTextureAtlas(path);
				}
			}
			ImGui::EndDragDropTarget();
		}

		if (!m_TextureAtlas)
		{
			ImGui::End();
			return;
		}

		if (m_TextureAtlas->GetTexture())
		{
			ImGui::Image((ImTextureID)m_TextureAtlas->GetTexture()->GetRendererID(), { 32.0f , 32.0f }, ImVec2(0, 1), ImVec2(1, 0));

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSETS_BROWSER_ITEM"))
				{
					std::string path((const char*)payload->Data);
					path.resize(payload->DataSize);
					TextureManager::GetInstance().AsyncLoad(path,
						[=](Texture* texture)
					{
						m_TextureAtlas->SetTexture(texture);
					});
				}
				ImGui::EndDragDropTarget();
			}
		}

		ImGui::Text("Name: %s", m_TextureAtlas->GetName().c_str());
		ImGui::Text("Filepath: %s", m_TextureAtlas->GetFilePath().c_str());

		glm::vec2 size = m_TextureAtlas->GetSize();
		if (ImGui::InputFloat2("Size", &size[0]))
		{
			m_TextureAtlas->SetSize(size);
		}

		if (ImGui::Button("Save"))
		{
			Serializer::SerializeTextureAtlas(m_TextureAtlas);
		}

		if (m_TextureAtlas->GetSize().x == 0 || m_TextureAtlas->GetSize().y == 0)
		{
			ImGui::End();
			return;
		}

		Texture* atlasTexture = m_TextureAtlas->GetTexture();
		int framePadding = -1;
		int amountOfSpritesX = (float)(atlasTexture->GetSize().x) / (float)(m_TextureAtlas->GetSize().x);
		int amountOfSpritesY = (float)(atlasTexture->GetSize().y) / (float)(m_TextureAtlas->GetSize().y);
		for (int i = amountOfSpritesY - 1; i >= 0; i--)
		{
			for (int j = 0; j < amountOfSpritesX; j++)
			{
				std::vector<float> uv = m_TextureAtlas->GetUV({ j, i });
				ImGui::PushID((i * amountOfSpritesX) + j);
				if (ImGui::ImageButton((void*)(intptr_t)atlasTexture->GetRendererID(), ImVec2(56, 56), ImVec2(uv[0], uv[5]), ImVec2(uv[4], uv[1]), framePadding, ImVec4(1.0f, 1.0f, 1.0f, 0.5f)))
				{
					for (const std::string& gameObjectUUID : Editor::GetInstance().m_SelectedGameObjects) {
						
						if (GameObject* gameObject = Editor::GetInstance().m_CurrentScene->FindGameObjectByUUID(gameObjectUUID))
						{
							Renderer2D* r2d = gameObject->m_ComponentManager.GetComponent<Renderer2D>();
							if (r2d)
							{
								r2d->SetTexture(atlasTexture);
								r2d->SetUV(uv);
							}
						}
					}
				}

				if (ImGui::BeginDragDropSource())
				{
					ImGui::SetDragDropPayload("TEXTURE_ATLAS", (const void*)(&TextureAtlasData(m_TextureAtlas, i, j)), 16);
					ImGui::EndDragDropSource();
				}

				ImGui::PopID();
				ImGui::SameLine();
			}
			ImGui::NewLine();
		}

		ImGui::End();
	}
}

Editor::Indent::Indent()
{
	ImGui::Indent();
}

Editor::Indent::~Indent()
{
	ImGui::Unindent();
}
