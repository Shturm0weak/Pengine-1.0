#pragma once

#include "Core.h"
#include "../Components/Transform.h"

namespace Pengine
{

	class PENGINE_API Camera
	{
	public:

		enum class CameraType
		{
			ORTHOGRAPHIC,
			PERSPECTIVE
		};
	private:

		glm::mat4 m_ViewMat4;
		glm::mat4 m_ProjectionMat4;
		
		glm::vec2 m_Size;
		float m_ZoomScale = 1.0f;
		float m_Znear;
		float m_Zfar;
		CameraType m_Type = CameraType::ORTHOGRAPHIC;

		void Copy(const Camera& camera);
		
		void UpdateViewProjection();
		
		void MoveOrthographic();

		friend class Editor;
		friend class Serializer;
	public:

		float m_Fov = glm::radians(90.0f);
		float m_Speed = 5.0f;
		float m_ZoomSensetivity = 0.25f;
		
		Transform m_Transform;
		glm::mat4 m_ViewProjectionMat4;
		
		Camera();
		Camera(const Camera& camera);
		void operator=(const Camera& camera);

		glm::mat4 GetViewProjectionMat4() const { return m_ViewProjectionMat4; }
		
		glm::mat4 GetProjectionMat4() const { return m_ProjectionMat4; }
		
		glm::mat4 GetViewMat4() const { return m_ViewMat4; }
		
		float GetPitch() const { return m_Transform.GetRotation().x; }
		
		float GetYaw() const { return m_Transform.GetRotation().y; }
		
		float GetRoll() const { return m_Transform.GetRotation().z; }
		
		CameraType GetType() const { return m_Type; }
		
		void SetOrthographic(const glm::vec2& size, float zNear = -1.0f, float zFar = 10000.0f);
		
		void SetPerspective(const glm::vec2& size, float fov, float zNear = 0.1f, float zFar = 10000.0f);
		
		void UpdateProjection(const glm::vec2& size);
		
		void Movement();
		
		void SetZoom(float zoom);
		
		float GetZoom() const { return m_ZoomScale; }
		
		float GetAspect() const { return m_Size.x / m_Size.y; }
		
		float GetFov() const { return m_Fov; }
	};

}