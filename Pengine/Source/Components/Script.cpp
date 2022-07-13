#include "Script.h"

#include "../Core/EntryPoint.h"
#include "../Lua/LuaStateManager.h"

using namespace Pengine;

IComponent* Script::CreateCopy(GameObject* newOwner)
{
	Script* script = new Script();
	script->m_Owner = newOwner;
	script->Copy(*this);

	if (EntryPoint::GetApplication().GetState() == Application::ApplicationState::Play)
	{
		Initialize();
	}

	return script;
}

IComponent* Script::New(GameObject* owner)
{
	return Create(owner);
}

void Script::Copy(const IComponent& component)
{
	Script& script = *(Script*)&component;
	m_Type = component.GetType();

	m_LStates.clear();
	for (auto& state : script.m_LStates)
	{
		m_LStates.push_back(LuaStateManager::GetInstance().Create(state->m_FilePath));
	}
}

IComponent* Script::Create(GameObject* owner)
{
	Script* script = new Script();

	if (EntryPoint::GetApplication().GetState() == Application::ApplicationState::Play)
	{
		script->Initialize();
	}

	return script;
}

LuaState* Script::Get(const std::string& filePath)
{
	for (auto& state : m_LStates)
	{
		if (state->m_FilePath == filePath)
		{
			return state;
		}
	}

	return nullptr;
}

void Script::Remove(const std::string& filePath)
{
	for (size_t i = 0; i < m_LStates.size(); i++)
	{
		LuaState* state = m_LStates[i];
		if (state->m_FilePath == filePath)
		{
			LuaStateManager::GetInstance().Remove(m_LStates[i]);
			m_LStates.erase(m_LStates.begin() + i);

			return;
		}
	}
}

void Script::Assign(const std::string& filePath)
{
	LuaState* state = Get(filePath);
	
	if (!state)
	{
		m_LStates.push_back(LuaStateManager::GetInstance().Create(filePath));
	}

	if (EntryPoint::GetApplication().GetState() == Application::ApplicationState::Play)
	{
		m_LStates.back()->Initialize(m_Owner);
	}
}

void Script::Initialize()
{
	for (auto& state : m_LStates)
	{
		state->Initialize(m_Owner);
	}
}

Script::~Script()
{
	for (auto& state : m_LStates)
	{
		LuaStateManager::GetInstance().Remove(state);
	}

	m_LStates.clear();
}