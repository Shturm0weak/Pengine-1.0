#include "Window.h"

#include "Environment.h"
#include "Input.h"
#include "Logger.h"
#include "Editor.h"
#include "../EventSystem/EventSystem.h"
#include "../Events/SetScrollEvent.h"
#include "../UI/Gui.h"

#include <glew.h>
#include <glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <glfw3native.h>

using namespace Pengine;

Window::~Window()
{
	ShutDown();
}

int Window::CreateWindowInstance(const std::string& title,
	const glm::ivec2& size, GLFWmonitor* monitor)
{
	if (!glfwInit())
	{
		Logger::Error("Failed to initialize GLFW!");

		return -1;
	}

	SetSize(size);

	m_Window = glfwCreateWindow(m_Size.x, m_Size.y, m_Title.c_str(), monitor, NULL);

	if (!m_Window)
	{
		Logger::Error("Failed to create window!");
		glfwTerminate();

		return -1;
	}

	glfwMakeContextCurrent(m_Window);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		Logger::Error("Failed to initialize GLEW!");

		return -1;
	}

	SetVSyncEnabled(m_VSync);

	m_ImGuiContext = ImGui::CreateContext();
	ImGui::SetCurrentContext(m_ImGuiContext);
	m_ImGuiIO = &ImGui::GetIO();
	ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	m_ImGuiIO->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	m_ImGuiIO->FontDefault = m_ImGuiIO->Fonts->AddFontFromFileTTF("Source/Fonts/OpenSans/OpenSans-Regular.ttf", 16.0f);

	glfwSetScrollCallback(m_Window,
		[](GLFWwindow* window, double xoffset, double yoffset) {
			glm::vec2 offset = { xoffset, yoffset };

			Window::GetInstance().AddScrollOffset(offset);
			EventSystem::GetInstance().SendEvent(new OnSetScrollEvent(offset, EventType::ONSETSCROLL));

			ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
		}
	);

	glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width,
		int height) {
			Window::GetInstance().SetSize({ width, height });
		}
	);

	Editor::GetInstance().SetDarkThemeColors();

	Logger::UpdateTime();
	Input::SetupCallBack();

	return 0;
}

int Window::Initialize(const std::string& title)
{
	m_Title = title;

	bool exitCode = SetWindowMode(m_WindowMode);

	Logger::Log(std::string("Hardware supports OpenGL: " + std::string((char*)glGetString(GL_VERSION))).c_str());
	Logger::Log(std::string("Application supports GLEW: " + std::string((char*)glewGetString(GLEW_VERSION))).c_str());
	Logger::Success("Window has been initialized!");

	return exitCode;
}

Window& Window::GetInstance()
{
	static Window window;
	return window;
}

glm::dvec2 Window::GetCursorPosition()
{
	m_PreviousCursorPosition = m_CursorPosition;
	glfwGetCursorPos(Window::GetWindow(), &m_CursorPosition.x, &m_CursorPosition.y);
	return m_CursorPosition;
}

void Window::SetWindowSize(const glm::ivec2 size)
{
	glfwSetWindowSize(m_Window, size.x, size.y);
}

void Window::Exit()
{
	glfwSetWindowShouldClose(m_Window, true);
}

bool Window::ShouldExit() const
{
	return !glfwWindowShouldClose(m_Window);
}

int Window::SetWindowMode(WindowMode windowMode)
{
	m_WindowMode = windowMode;

	bool exitCode = 0;

	switch (m_WindowMode)
	{
	case WindowMode::Windowed:
	{
		if (!m_Window)
		{
			exitCode = CreateWindowInstance(m_Title, { 1280, 720 }, NULL);
		}

		glfwSetWindowMonitor(m_Window, NULL, 1280 * 0.25, 720 * 0.25, 1280, 720, 60);

		break;
	}
	case WindowMode::Fullscreen:
	{
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);

		if (!m_Window)
		{
			exitCode = CreateWindowInstance(m_Title, { 1280, 720 }, NULL);
		}

		glfwWindowHint(GLFW_RED_BITS, mode->redBits);
		glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
		glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
		glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

		glfwSetWindowMonitor(m_Window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);

		break;
	}
	default:
		break;
	}

	return exitCode;
}

void Window::SetTitle(const std::string& title)
{
	glfwSetWindowTitle(m_Window, title.c_str());
}

void Window::NewFrame()
{
	SetScrollOffset({ 0.0f, 0.0f });
	GetCursorPosition();
	Editor::GetInstance().ResetStats();
	EventSystem::GetInstance().SendEvent(new IEvent(EventType::ONUPDATE));
	Window::GetInstance().Clear();
	Logger::UpdateTime();
	Time::CalculateDeltaTime();
	Input::ResetInput();
	Gui::GetInstance().Begin();
	glfwPollEvents();
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void Window::EndFrame() const
{
	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	glfwSwapBuffers(m_Window);
}

void Window::SetSize(glm::ivec2 size)
{
	if (m_Size == size)
	{
		return;
	}

	m_Size = size;
	
	EventSystem::GetInstance().SendEvent(new WindowResizeEvent(m_Size, EventType::ONWINDOWRESIZE));
}

void Window::SetTitle(const char* title)
{
	glfwSetWindowTitle(m_Window, title);
}

void Window::SetCurrentContext() const
{
	ImGui::SetCurrentContext(m_ImGuiContext);
}

void Window::SetVSyncEnabled(bool enabled)
{
	m_VSync = enabled;
	glfwSwapInterval(m_VSync);
}

void Window::Clear(const glm::vec4& color) const
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearDepth(1.0f);
	glClearColor(color.x, color.y, color.y, color.w);
}

void Window::ClampCursorPosition()
{
	glm::dvec2 cursorPosition = GetCursorPosition();
	glm::dvec2 newCursorPos = cursorPosition;
	if (cursorPosition.x > m_Size.x)
	{
		newCursorPos.x = 0.0f;
	}
	else if (cursorPosition.x < 0.0f)
	{
		newCursorPos.x = m_Size.x;
	}
	if (cursorPosition.y > m_Size.y)
	{
		newCursorPos.y = 0.0f;
	}
	else if (cursorPosition.y < 0.0f)
	{
		newCursorPos.y = m_Size.y;
	}

	glfwSetCursorPos(m_Window, newCursorPos.x, newCursorPos.y);
}

void Window::ShutDown()
{
	glfwTerminate();
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void Window::DisableCursor()
{
	glfwSetInputMode(Window::GetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

int Window::GetCursorMode()
{
	return glfwGetInputMode(Window::GetWindow(), GLFW_CURSOR);
}

void Window::ShowCursor()
{
	glfwSetInputMode(Window::GetWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void Window::HideCursor()
{
	glfwSetInputMode(Window::GetWindow(), GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}