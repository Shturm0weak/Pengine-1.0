#pragma once

#include "Core.h"
#include "Camera.h"

#include <memory>

namespace Pengine
{

	class PENGINE_API Environment
	{
	private:

		std::shared_ptr<class Camera> m_EditorCamera;
		std::shared_ptr<class Camera> m_MainCamera;

		Environment() = default;
		Environment(const Environment&) = delete;
		Environment& operator=(const Environment&) { return *this; }
		~Environment() = default;
	public:

		static Environment& GetInstance();

		void UseEditorCameraAsMain() { m_MainCamera = m_EditorCamera; }
		std::shared_ptr<Camera> GetMainCamera() const { return m_MainCamera; }
		void SetMainCamera(const std::shared_ptr<class Camera>& camera) { m_MainCamera = camera; }
		void SetEditorCamera(const std::shared_ptr<class Camera>& camera) { m_EditorCamera = camera; }
	};

}