#include "Window.h"

#include "Environment.h"
#include "Input.h"
#include "Logger.h"
#include "Editor.h"
#include "../EventSystem/EventSystem.h"

#include <glew.h>
#include <glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <glfw3native.h>

using namespace Pengine;

Window::~Window()
{
	glfwTerminate();
}

int Window::Initialize(const std::string& title)
{
	if (!glfwInit())
	{
		return -1;
	}

	m_Window = glfwCreateWindow(m_Size.x, m_Size.y, title.c_str(), NULL, NULL);

	if (!m_Window)
	{
		Logger::Error("Failed to initialize GLFW!");
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
	m_ImGuiIO = &ImGui::GetIO();
	ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
	m_ImGuiIO->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	m_ImGuiIO->FontDefault = m_ImGuiIO->Fonts->AddFontFromFileTTF("Source/Fonts/OpenSans/OpenSans-Regular.ttf", 16.0f);

	glfwSetScrollCallback(Window::GetInstance().GetWindow(),
		[](GLFWwindow* window, double xoffset, double yoffset) {
			Window::GetInstance().AddScrollOffset({ xoffset, yoffset });
			Environment::GetInstance().GetMainCamera()->SetZoom(yoffset);

			ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
		}
	);

	glfwSetWindowSizeCallback(Window::GetWindow(), [](GLFWwindow* window, int width,
		int height) {
			Window::GetInstance().SetSize({ width, height });
		}
	);

	Editor::GetInstance().SetDarkThemeColors();

	Logger::UpdateTime();
	Input::SetupCallBack();

	Logger::Log(std::string("Hardware supports OpenGL: " + std::string((char*)glGetString(GL_VERSION))).c_str());
	Logger::Log(std::string("Application supports GLEW: " + std::string((char*)glewGetString(GLEW_VERSION))).c_str());
	Logger::Success("Window has been initialized!");

	return 0;
}

Window& Window::GetInstance()
{
	static Window window;
	return window;
}

void Window::Exit()
{
	glfwSetWindowShouldClose(m_Window, true);
}

bool Window::ShouldExit() const
{
	return !glfwWindowShouldClose(m_Window);
}

void Window::SetTitle(const std::string& title)
{
	glfwSetWindowTitle(m_Window, title.c_str());
}

void Window::NewFrame()
{
	SetScrollOffset({ 0.0f, 0.0f });
	Editor::GetInstance().ResetStats();
	EventSystem::GetInstance().SendEvent(new IEvent(EventType::ONUPDATE));
	Window::GetInstance().Clear();
	Logger::UpdateTime();
	Time::CalculateDeltaTime();
	Input::ResetInput();
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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glClearDepth(1.0f);
	glClearColor(color.x, color.y, color.y, color.w);
}
