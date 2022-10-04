#pragma once

#include "Core.h"

namespace Pengine
{

	class PENGINE_API Viewport
	{
	private:

		glm::ivec2 m_Size;
		glm::ivec2 m_PreviousWindowSize;
		glm::ivec2 m_Position;
		glm::vec2 m_PreviousMousePosition;
		glm::vec2 m_PreviousUIMousePosition;
		glm::vec2 m_NormalizedMousePosition;
		glm::vec2 m_MousePosition;
		glm::vec2 m_UIMousePosition;
		ImVec2 m_ImGuiMousePosition;

		class FrameBuffer* m_FrameBufferViewport = nullptr;

		int m_GizmoOperation = -1;

		bool m_IsOpen = true;
		bool m_IsActiveGuizmo = false;
		bool m_IsHovered = false;
		bool m_IsFocused = false;
		bool m_IsClamped = false;

		int m_MipMap = 0;

		void SetPosition(const glm::ivec2 position);
		
		void Initialize();

		Viewport() = default;
		Viewport(const Viewport&) = delete;
		Viewport& operator=(const Viewport&) { return *this; }
		~Viewport() = default;

		friend class EntryPoint;
		friend class Renderer;
	public:

		static Viewport& GetInstance();

		glm::ivec2 GetPosition() const { return m_Position; }

		glm::ivec2 GetSize() const { return m_Size; }
		
		glm::vec2 GetMousePosition() const { return m_MousePosition; }
		
		glm::vec2 GetPreviousMousePosition() const { return m_PreviousMousePosition; }

		glm::vec2 GetUIMousePosition() const { return m_UIMousePosition; }
		
		glm::vec2 GetPreviousUIMousePosition() const { return m_PreviousUIMousePosition; }

		glm::vec2 GetUIDragDelta() const { return !m_IsClamped ? m_UIMousePosition - m_PreviousUIMousePosition : glm::vec2(0.0f, 0.0f); }
		
		glm::vec2 GetDragDelta() const { return !m_IsClamped ? m_MousePosition - m_PreviousMousePosition : glm::vec2(0.0f, 0.0f); }
		
		glm::vec2 GetMousePositionFromWorldToNormalized(const glm::vec2& position) const;
		
		glm::vec2 GetNormalizedMousePosition() const { return m_NormalizedMousePosition; }

		float GetAspect() const { return (float)m_Size.x / (float)m_Size.y; }
		
		void Begin();
		
		void End();
		
		void Update();
		
		void ShutDown();

		void Resize(const glm::ivec2& size);
		
		void ClampCursor();

		bool IsHovered() const { return m_IsHovered; }
		
		bool IsFocused() const { return m_IsFocused; }
		
		bool IsActiveGuizmo() const { return m_IsActiveGuizmo; }
	};

}
