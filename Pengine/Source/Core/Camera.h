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
		glm::mat4 m_ViewProjectionMat4;
		
		glm::vec2 m_Size;

		float m_ZoomScale = 1.0f;
		float m_Znear = 0.1f;
		float m_Zfar = 1000.0f;
		float m_Fov = glm::radians(90.0f);
		float m_Speed = 5.0f;
		float m_ZoomSensetivity = 0.25f;
		
		bool m_IsMoving = false;

		CameraType m_Type = CameraType::ORTHOGRAPHIC;

		void Copy(const Camera& camera);
		
		void UpdateViewProjection();
		
		void MoveOrthographic();

		void MovePerspective();

		friend class Editor;
		friend class Serializer;
	public:
		Transform m_Transform;
		
		Camera();
		Camera(const Camera& camera);
		void operator=(const Camera& camera);

		glm::mat4 GetViewProjectionMat4() const { return m_ViewProjectionMat4; }
		
		glm::mat4 GetProjectionMat4() const { return m_ProjectionMat4; }
		
		glm::mat4 GetViewMat4() const { return m_ViewMat4; }
		
		glm::vec3 GetMouseDirection();

		glm::vec2 GetSize() const { return m_Size; }

		void SetSize(const glm::vec2& size);

		float GetPitch() const { return m_Transform.GetRotation().x; }
		
		float GetYaw() const { return m_Transform.GetRotation().y; }
		
		float GetRoll() const { return m_Transform.GetRotation().z; }
		
		CameraType GetType() const { return m_Type; }
		
		void SetOrthographic(const glm::vec2& size);
		
		void SetPerspective(const glm::vec2& size);
		
		void SetType(CameraType type);

		void UpdateProjection();
		
		void Movement();
		
		void SetZoom(float zoom);
		
		float GetZoom() const { return m_ZoomScale; }
		
		float GetAspect() const { return m_Size.x / m_Size.y; }
		
		float GetFov() const { return m_Fov; }
		
		void SetFov(float fov);

		float GetZNear() const { return m_Znear; }

		void SetZNear(float zNear);

		float GetZFar() const { return m_Zfar; }
		
		void SetZFar(float zFar);
	};

}