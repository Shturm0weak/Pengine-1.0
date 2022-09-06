#pragma once

#include "Core/Core.h"
#include "Core/Application.h"
#include "Audio/SoundManager.h"

using namespace Pengine;

class KingsAndPigs : public Application
{
private:

	Sound* m_AmbientSound = nullptr;
public:

	KingsAndPigs(const std::string& title = "KingsAndPigs")
		: Application(title)
	{
		m_AmbientSound = SoundManager::GetInstance().Load("Source/Examples/KingsAndPigs/Sounds/Ambient.ogg");
	}

	virtual void OnStart() override 
	{
		m_AmbientSound->SetVolume(0.05f);
		SoundManager::GetInstance().Loop(m_AmbientSound);

		GameObject* gameObject = GetScene()->CreateGameObject();
		gameObject->m_ComponentManager.AddComponent<Renderer3D>()->SetMesh(MeshManager::GetInstance().Load("Source/Meshes/Cube.obj"));
	};

	virtual void OnPostStart() override {};

	virtual void OnUpdate() override {};

	virtual void OnGuiRender() override {};

	virtual void OnImGuiRender() override {};

	virtual void OnClose() override 
	{
		SoundManager::GetInstance().Stop(m_AmbientSound);
	};
};