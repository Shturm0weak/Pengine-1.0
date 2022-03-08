#pragma once

#include "Core.h"

#include "Scene.h"

#include <vector>

namespace Pengine
{

	class PENGINE_API Application
	{
	public:

		enum class ApplicationState
		{
			Play = 0,
			Edit,
			Paused
		};
	private:
		
		Scene* m_Scene = new Scene("Scene");
		Scene* m_DefaultScene = new Scene("DefaultScene");
		ApplicationState m_ApplicationState = ApplicationState::Edit;

		void OnPlay();
		void OnStop();

		friend class Editor;
	public:

		virtual void OnStart() {};
		virtual void OnUpdate() {};
		virtual void OnGuiRender() {};
		virtual void OnClose() {};
		virtual ~Application() {};
		Application() = default;

		Scene* GetScene() const { return m_Scene; }
		Scene* NewScene(const std::string& title);
		ApplicationState GetState() const { return m_ApplicationState; }
		void SetState(ApplicationState applicationState) { m_ApplicationState = applicationState; }

		void UpdatePhysics();
		void ShutDown();
		void Render();
	};

}