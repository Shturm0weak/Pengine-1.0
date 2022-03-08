#pragma once

#include "Core.h"

#include <glew.h>
#include <glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <glfw3native.h>

namespace Pengine
{

	class PENGINE_API Window
	{
	private:

		GLFWwindow* m_Window = nullptr;

		glm::ivec2 m_Size = { 800, 600 };
		glm::vec2 m_ScrollOffset = { 0.0f, 0.0f };

		ImGuiContext* m_ImGuiContext = nullptr;
		ImGuiIO* m_ImGuiIO = nullptr;

		bool m_VSync = false;

		Window() = default;
		Window(const Window&) = delete;
		Window& operator=(const Window&) { return *this; }
		~Window();

		friend class Editor;
	public:

		static Window& GetInstance();

		int Initialize();
		glm::ivec2 GetSize() const { return m_Size; }
		GLFWwindow* GetWindow() const { return m_Window; }
		glm::vec2 GetScrollOffset() const { return m_ScrollOffset; }
		bool ShouldExit() const;
		void NewFrame() const;
		void EndFrame() const;
		void SetSize(glm::ivec2 size);
		void SetTitle(const char* title);
		void SetCurrentContext() const;
		void SetVSyncEnabled(bool enabled) { m_VSync = enabled; }
		void SetScrollOffset(const glm::vec2& offset) { m_ScrollOffset = offset; }
		void AddScrollOffset(const glm::vec2& offset) { m_ScrollOffset += offset; }
		void Clear() const;
	};

}