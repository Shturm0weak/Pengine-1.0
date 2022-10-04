#pragma once

#include "Core.h"
#include "Camera.h"
#include "../EventSystem/Listener.h"

#include <memory>

namespace Pengine
{

	class PENGINE_API Environment : public IListener
	{
	private:

		struct BloomSettings
		{
			float m_BrightnessThreshold = 1.1f;
			float m_Gamma = 1.0f;
			float m_Exposure = 0.05f;
			int m_BlurPasses = 8;
			int m_PixelsBlured = 6;
			bool m_IsEnabled = true;
		} m_BloomSettings;

		std::shared_ptr<class Camera> m_EditorCamera;
		std::shared_ptr<class Camera> m_MainCamera;

		float m_GlobalIntensity = 1.0f;
	
		bool m_DepthTest = true;

		Environment();
		Environment(const Environment&) = delete;
		Environment& operator=(const Environment&) { return *this; }
		~Environment() = default;

		friend class Editor;
		friend class Renderer;
		friend class Serializer;
		friend class Application;
	public:

		static Environment& GetInstance();

		virtual void OnSetScroll(const OnSetScrollEvent& event);

		void UseEditorCameraAsMain() { m_MainCamera = m_EditorCamera; }
		
		std::shared_ptr<Camera> GetMainCamera() const { return m_MainCamera; }
		
		void SetMainCamera(const std::shared_ptr<class Camera>& camera) { m_MainCamera = camera; }
		
		void SetEditorCamera(const std::shared_ptr<class Camera>& camera) { m_EditorCamera = camera; }

		void SetGlobalIntensity(float globalIntensity = 1.0f) { m_GlobalIntensity = glm::clamp(globalIntensity, 0.0f, FLT_MAX); }
	
		float GetGlobalIntensity() const { return m_GlobalIntensity; }

		void SetDepthTest(bool depthTest);
	};

}