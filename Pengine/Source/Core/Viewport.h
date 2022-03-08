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
		glm::vec2 m_MousePosition;
		glm::vec2 m_UIMousePosition;

		class FrameBuffer* m_FrameBufferViewport = nullptr;

		int m_GizmoOperation = -1;

		bool m_IsOpen = true;
		bool m_IsActiveGuizmo = false;
		bool m_IsHovered = false;
		bool m_IsFocused = false;

		void SetViewPortPosition(const glm::ivec2 position);
		void Initialize();

		Viewport() = default;
		Viewport(const Viewport&) = delete;
		Viewport& operator=(const Viewport&) { return *this; }
		~Viewport() = default;

		friend class EntryPoint;
	public:

		static Viewport& GetInstance();

		glm::ivec2 GetSize() const { return m_Size; }
		glm::vec2 GetMousePosition() const { return m_MousePosition; }
		glm::vec2 GetUIMousePosition() const { return m_UIMousePosition; }
		glm::vec2 GetDragDelta() const { return m_MousePosition - m_PreviousMousePosition; }
		float GetAspect() const { return (float)m_Size.x / (float)m_Size.y; }
		void Begin();
		void End();
		void Update();
		void Resize(const glm::ivec2& size);
		bool IsHovered() const { return m_IsHovered; }
		bool IsFocused() const { return m_IsFocused; }
		bool IsActiveGuizmo() const { return m_IsActiveGuizmo; }
	};

}
