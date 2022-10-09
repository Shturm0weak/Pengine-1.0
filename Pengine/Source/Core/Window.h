#pragma once

#include "Core.h"

namespace Pengine
{

	class PENGINE_API Window
	{
	private:

		enum class WindowMode
		{
			Windowed = 0,
			Fullscreen = 1
		} m_WindowMode = WindowMode::Windowed;

		GLFWwindow* m_Window = nullptr;

		glm::ivec2 m_Size = { 800, 600 };
		glm::vec2 m_ScrollOffset = { 0.0f, 0.0f };

		glm::dvec2 m_CursorPosition;
		glm::dvec2 m_PreviousCursorPosition;

		std::string m_Title = "Application";

		ImGuiContext* m_ImGuiContext = nullptr;
		ImGuiIO* m_ImGuiIO = nullptr;

		bool m_VSync = false;

		Window() = default;
		Window(const Window&) = delete;
		Window& operator=(const Window&) { return *this; }
		~Window();

		int CreateWindowInstance(const std::string& title,
			const glm::ivec2& size, GLFWmonitor* monitor);

		friend class Editor;
	public:

		static Window& GetInstance();

		int Initialize(const std::string& title);
		
		ImGuiContext* GetImGuiContext() const { return m_ImGuiContext; }

		glm::ivec2 GetSize() const { return m_Size; }
		
		GLFWwindow* GetWindow() const { return m_Window; }
		
		glm::vec2 GetScrollOffset() const { return m_ScrollOffset; }
		
		glm::dvec2 GetCursorPosition();

		glm::dvec2 GetPreviousCursorPosition() const { return m_PreviousCursorPosition; }

		void SetWindowSize(const glm::ivec2 size);

		void Exit();

		bool ShouldExit() const;
		
		int SetWindowMode(WindowMode windowMode);

		void SetTitle(const std::string& title);
		
		void NewFrame();
		
		void EndFrame() const;
		
		void SetSize(glm::ivec2 size);
		
		void SetTitle(const char* title);
		
		void SetCurrentContext() const;
		
		void SetVSyncEnabled(bool enabled);
		
		void SetScrollOffset(const glm::vec2& offset) { m_ScrollOffset = offset; }
		
		void AddScrollOffset(const glm::vec2& offset) { m_ScrollOffset += offset; }
		
		void Clear(const glm::vec4& color = { 0.0f, 0.0f, 0.0f, 1.0f }, float depth = 1.0f) const;

		void ClearColor(const glm::vec4& color = { 0.0f, 0.0f, 0.0f, 1.0f }) const;

		void ClearDepth(float depth = 1.0f) const;
		
		void ClampCursorPosition();

		void ShutDown();

		void DisableCursor();
		
		void ShowCursor();
		
		void HideCursor();

		int GetCursorMode();
	};

}