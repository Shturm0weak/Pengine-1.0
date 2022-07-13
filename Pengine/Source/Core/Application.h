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
	protected:
		
		Scene* m_Scene = new Scene("Scene");
		Scene* m_DefaultScene = new Scene("DefaultScene");
		std::string m_Title = "Application";
		ApplicationState m_ApplicationState = ApplicationState::Edit;
		bool m_IsPostStarted = false;
		bool m_IsStarted = false;

		void OnPlay();
		
		void OnStop();

		friend class Editor;
		friend class EntryPoint;
		friend class Renderer;
	public:

		Application(const std::string& title = "Application") : m_Title(title) {};
		
		virtual void OnStart() {};
		
		virtual void OnPostStart() {};
		
		virtual void OnUpdate() {};
		
		virtual void OnGuiRender() {};
		
		virtual void OnImGuiRender() {};

		virtual void OnClose() {};
		
		virtual ~Application() {};

		std::string GetTitle() const { return m_Title; }

		Scene* GetScene() const { return m_Scene; }
		
		Scene* NewScene(const std::string& title);
		
		ApplicationState GetState() const { return m_ApplicationState; }
		
		void SetState(ApplicationState applicationState) { m_ApplicationState = applicationState; }

		void OnLuaStart();
		
		void OnLuaUpdate();
		
		void PostStartCall();
		
		void UpdatePhysics();
		
		void ShutDown();

		bool IsStarted() const { return m_IsStarted; }
		
		bool IsPostStarted() const { return m_IsPostStarted; }
	};

}