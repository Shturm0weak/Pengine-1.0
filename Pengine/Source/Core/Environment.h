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
			float m_Exposure = 0.05f;
			int m_BlurPasses = 3;
			int m_PixelsBlured = 6;
			bool m_IsEnabled = true;
		} m_BloomSettings;

		struct ShadowsSettings
		{
			int m_MaxPointLightShadows = 8;
			int m_MaxSpotLightShadows = 8;
			bool m_IsEnabled = true;
			bool m_IsVisualized = false;

			struct Blur
			{
				int m_BlurPasses = 1;
				int m_PixelsBlured = 6;
			} m_Blur;
		} m_ShadowsSettings;

		struct SSAOSettings
		{
			int m_KernelSize = 10;
			float m_Radius = 0.5f;
			float m_Bias = 0.0f;
			int m_NoiseTextureDimension = 4;

			struct Blur
			{
				int m_BlurPasses = 1;
				int m_PixelsBlured = 6;
			} m_Blur;
			
			bool m_IsEnabled = true;
		} m_SSAO;

		std::shared_ptr<class Camera> m_EditorCamera;
		std::shared_ptr<class Camera> m_MainCamera;
	
		float m_Gamma = 2.2f;

		bool m_DepthTest = true;

		Environment();
		Environment(const Environment&) = delete;
		Environment& operator=(const Environment&) { return *this; }
		~Environment() = default;

		friend class Editor;
		friend class Renderer;
		friend class Renderer3D;
		friend class Serializer;
		friend class Instancing;
		friend class Application;
	public:

		static Environment& GetInstance();

		virtual void OnSetScroll(const OnSetScrollEvent& event);

		void UseEditorCameraAsMain() { m_MainCamera = m_EditorCamera; }
		
		std::shared_ptr<Camera> GetMainCamera() const { return m_MainCamera; }
		
		void SetMainCamera(const std::shared_ptr<class Camera>& camera) { m_MainCamera = camera; }
		
		void SetEditorCamera(const std::shared_ptr<class Camera>& camera) { m_EditorCamera = camera; }

		void SetDepthTest(bool depthTest);
	};

}