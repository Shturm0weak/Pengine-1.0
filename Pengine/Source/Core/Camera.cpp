#include "Camera.h"

#include "Window.h"
#include "Viewport.h"
#include "Input.h"
#include "Environment.h"
#include "../EventSystem/EventSystem.h"

using namespace Pengine;

void Camera::Copy(const Camera& camera)
{
	m_ProjectionMat4 = camera.m_ProjectionMat4;
	m_ViewProjectionMat4 = camera.m_ViewProjectionMat4;
	m_ZoomScale = camera.m_ZoomScale;
	m_Fov = camera.m_Fov;
	m_Type = camera.m_Type;
	m_Transform = camera.m_Transform;
}

void Camera::UpdateViewProjection()
{
	m_ViewMat4 = glm::inverse(m_Transform.GetTransform());
	m_ViewProjectionMat4 = m_ProjectionMat4 * m_ViewMat4;
}

void Camera::MoveOrthographic()
{
	if (Input::KeyBoard::IsKeyDown(Keycode::KEY_UP))
	{
		m_Transform.Translate(m_Transform.GetPosition()
			+ glm::vec3(0.0f, m_Speed * Time::GetDeltaTime(), 0.0f));
	}
	else if (Input::KeyBoard::IsKeyDown(Keycode::KEY_DOWN))
	{
		m_Transform.Translate(m_Transform.GetPosition()
			- glm::vec3(0.0f, m_Speed * Time::GetDeltaTime(), 0.0f));
	}

	if (Input::KeyBoard::IsKeyDown(Keycode::KEY_RIGHT))
	{
		m_Transform.Translate(m_Transform.GetPosition()
			+ glm::vec3(m_Speed * Time::GetDeltaTime(), 0.0f, 0.0f));
	}
	else if (Input::KeyBoard::IsKeyDown(Keycode::KEY_LEFT))
	{
		m_Transform.Translate(m_Transform.GetPosition()
			- glm::vec3(m_Speed * Time::GetDeltaTime(), 0.0f, 0.0f));
	}
}

Camera::Camera()
{
	m_Transform.SetOnRotationCallback(std::bind(&Camera::UpdateViewProjection, this));
	m_Transform.SetOnTranslationCallback(std::bind(&Camera::UpdateViewProjection, this));
	SetOrthographic(Viewport::GetInstance().GetSize());
}

Camera::Camera(const Camera& camera)
{
	Copy(camera);
}

void Camera::operator=(const Camera& camera)
{
	Copy(camera);
}

void Camera::SetOrthographic(const glm::vec2& size, float zNear, float zFar)
{
	m_Size = size;
	m_Zfar = zFar;
	m_Znear = zNear;
	const float ratio = m_Size.x / m_Size.y;
	m_ProjectionMat4 = glm::ortho(-ratio * m_ZoomScale, ratio * m_ZoomScale, -1.0f * m_ZoomScale,
		1.0f * m_ZoomScale, m_Znear, m_Zfar);
	m_Type = CameraType::ORTHOGRAPHIC;
	UpdateViewProjection();
}

void Camera::SetPerspective(const glm::vec2& size, float fov, float zNear, float zFar)
{
	m_Size = size;
	m_Zfar = zFar;
	m_Znear = zNear;
	m_Fov = fov;
	m_ProjectionMat4 = glm::perspective(m_Fov, m_Size.x / m_Size.y, m_Znear, m_Zfar);
	m_Type = CameraType::PERSPECTIVE;
	UpdateViewProjection();
}

void Camera::UpdateProjection(const glm::vec2& size)
{
	switch (m_Type)
	{
	case CameraType::PERSPECTIVE:
	{
		SetPerspective(size, m_Fov, m_Znear, m_Zfar);
		break;
	}
	case CameraType::ORTHOGRAPHIC:
	default:
	{
		SetOrthographic(size, m_Znear, m_Zfar);
		break;
	}
	}
}

void Camera::Movement()
{
	//Viewport::GetInstance().IsFocused() == false;
	if (Viewport::GetInstance().IsHovered() == false)
	{
		return;
	}

	switch (m_Type)
	{
	case CameraType::PERSPECTIVE:
	{
		break;
	}
	case CameraType::ORTHOGRAPHIC:
	default:
	{
		MoveOrthographic();
		break;
	}
	}
}

void Camera::SetZoom(float zoom)
{
	//Viewport::GetInstance().IsFocused();
	if (Viewport::GetInstance().IsHovered())
	{
		m_ZoomScale -= zoom * m_ZoomSensetivity;
		m_ZoomScale = glm::clamp(m_ZoomScale, 0.0f, 1000.f);
		UpdateProjection(m_Size);
	}
}
