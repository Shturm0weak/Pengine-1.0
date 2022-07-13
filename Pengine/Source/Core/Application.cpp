#include "Application.h"

#include "Editor.h"
#include "Renderer.h"
#include "Viewport.h"
#include "../Lua/LuaStateManager.h"
#include "../Lua/LuaState.h"

using namespace Pengine;

void Application::OnPlay()
{
	if (m_IsStarted) return;

	SetState(Application::ApplicationState::Play);
	m_IsPostStarted = false;
	m_DefaultScene->Clear();
	*m_DefaultScene = *m_Scene;
	OnStart();
	OnLuaStart();
	m_Scene->OnPhysicsStart();
	m_IsStarted = true;
}

void Application::OnStop()
{
	SetState(Application::ApplicationState::Edit);
	m_Scene->OnPhysicsClose();
	OnClose();
	m_Scene->Clear();
	*m_Scene = *m_DefaultScene;
	m_DefaultScene->Clear();
	m_IsStarted = false;
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

void Application::OnLuaStart()
{
	for (auto& state : LuaStateManager::GetInstance().m_LuaStates)
	{
		if (state->m_L && state->m_IsEnabled)
		{
			state->OnStart();
		}
	}
}

void Application::OnLuaUpdate()
{
	for (auto& state : LuaStateManager::GetInstance().m_LuaStates)
	{
		if (state->m_L && state->m_IsEnabled)
		{
			state->OnUpdate(Time::GetDeltaTime());
		}
	}
}

void Application::PostStartCall()
{
	if (!m_IsPostStarted)
	{
		OnPostStart();
		m_IsPostStarted = true;
	}
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
