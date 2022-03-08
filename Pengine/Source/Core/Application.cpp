#include "Application.h"

#include "Timer.h"
#include "Editor.h"

using namespace Pengine;

void Application::OnPlay()
{
	m_DefaultScene->Clear();
	*m_DefaultScene = *m_Scene;
	SetState(Application::ApplicationState::Play);
	m_Scene->OnPhysicsStart();
	OnStart();
}

void Application::OnStop()
{
	SetState(Application::ApplicationState::Edit);
	m_Scene->OnPhysicsClose();
	OnClose();
	m_Scene->Clear();
	*m_Scene = *m_DefaultScene;
	m_DefaultScene->Clear();
}

Scene* Application::NewScene(const std::string& title)
{
	if (m_Scene != nullptr)
	{
		delete m_Scene;
	}

	m_Scene = new Scene(title);

	return m_Scene;
}

void Application::UpdatePhysics()
{
	m_Scene->OnPhysicsUpdate();
}

void Application::ShutDown()
{
	m_Scene->ShutDown();
	m_DefaultScene->ShutDown();
	delete m_Scene;
	delete m_DefaultScene;
}

void Application::Render()
{
	Timer timer = Timer(false, &Editor::GetInstance().m_Stats.m_RenderTime);
	m_Scene->Render();
}
