#include "Camera.h"

#include "Window.h"
#include "Viewport.h"
#include "Input.h"
#include "Environment.h"
#include "Logger.h"
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

void Camera::MovePerspective()
{
	if (Viewport::GetInstance().IsHovered()
		&& Input::Mouse::IsMousePressed(Keycode::MOUSE_BUTTON_2))
	{
		Window::GetInstance().DisableCursor();
		m_IsMoving = true;
	}

	if (!m_IsMoving) return;

	Viewport::GetInstance().ClampCursor();

	glm::vec3 rotation = m_Transform.GetRotation();
	if (rotation.y > glm::two_pi<float>() || rotation.y < -glm::two_pi<float>())
	{
		rotation.y = 0.0f;
	}
	if (rotation.x > glm::two_pi<float>() || rotation.x < -glm::two_pi<float>())
	{
		rotation.x = 0.0f;
	}
	
	m_Transform.Rotate(glm::vec3(rotation.x, rotation.y, 0.0f));

	float speed = m_Speed;
	if (Input::KeyBoard::IsKeyDown(Keycode::KEY_LEFT_SHIFT))
	{
		speed *= 10.0f;
	}

	float rotationSpeed = 1.0f;

	glm::vec3 back = m_Transform.GetBack();

	if (Input::KeyBoard::IsKeyDown(Keycode::KEY_W))
	{
		m_Transform.Translate(m_Transform.GetPosition() +
			glm::vec3(-back.x, back.y, -back.z) * Time::GetDeltaTime() * speed);
	}
	if (Input::KeyBoard::IsKeyDown(Keycode::KEY_S))
	{
		m_Transform.Translate(m_Transform.GetPosition() +
			glm::vec3(back.x, -back.y, back.z) * Time::GetDeltaTime() * speed);
	}

	glm::vec2 delta = Viewport::GetInstance().GetUIDragDelta() * 0.1f;

	m_Transform.Rotate(glm::vec3(rotation.x + glm::radians(delta.y),
		rotation.y - glm::radians(delta.x), 0));

	UpdateViewProjection();

	if (Input::KeyBoard::IsKeyDown(Keycode::KEY_A))
	{
		m_Transform.Translate(m_Transform.GetPosition() -
			m_Transform.GetRight() * Time::GetDeltaTime() * speed);
	}
	if (Input::KeyBoard::IsKeyDown(Keycode::KEY_D))
	{
		m_Transform.Translate(m_Transform.GetPosition() +
			m_Transform.GetRight() * Time::GetDeltaTime() * speed);
	}
	if (Input::KeyBoard::IsKeyDown(Keycode::KEY_E))
	{
		m_Transform.Translate(m_Transform.GetPosition() +
			m_Transform.GetUp() * Time::GetDeltaTime() * speed);
	}
	if (Input::KeyBoard::IsKeyDown(Keycode::KEY_Q))
	{
		m_Transform.Translate(m_Transform.GetPosition() -
			m_Transform.GetUp() * Time::GetDeltaTime() * speed);
	}
}

Camera::Camera()
{
	m_Transform.SetOnRotationCallback("Camera", std::bind(&Camera::UpdateViewProjection, this));
	m_Transform.SetOnTranslationCallback("Camera", std::bind(&Camera::UpdateViewProjection, this));
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

glm::vec3 Camera::GetMouseDirection()
{
	glm::dvec2 normalizedMousePosition = Viewport::GetInstance().GetNormalizedMousePosition();
	glm::dvec4 clipCoords = glm::dvec4(normalizedMousePosition.x, normalizedMousePosition.y, -1.0f, 1.0f);
	glm::dvec4 eyeCoords = clipCoords * glm::inverse(m_ProjectionMat4);
	eyeCoords.z = -1.0f; eyeCoords.w = 0.0f;
	glm::dvec3 mouseRay = (glm::dvec3)(glm::inverse(m_ViewMat4) * eyeCoords);
	return glm::normalize(mouseRay);
}

void Camera::SetSize(const glm::vec2& size)
{
	m_Size = size;

	UpdateProjection();
}

void Camera::SetOrthographic(const glm::vec2& size)
{
	m_Size = size;
	const float ratio = GetAspect();
	m_ProjectionMat4 = glm::ortho(-ratio * m_ZoomScale, ratio * m_ZoomScale, -1.0f * m_ZoomScale,
		1.0f * m_ZoomScale, m_Znear, m_Zfar);
	m_Type = CameraType::ORTHOGRAPHIC;
	UpdateViewProjection();
}

void Camera::SetPerspective(const glm::vec2& size)
{
	if (size.x == 0 || size.y == 0)
	{
		m_Size = { 1.0f, 1.0f };
	}
	else
	{
		m_Size = size;
	}

	m_ProjectionMat4 = glm::perspective(m_Fov, GetAspect(), m_Znear, m_Zfar);
	m_Type = CameraType::PERSPECTIVE;
	UpdateViewProjection();
}

void Camera::SetType(CameraType type)
{
	switch (type)
	{
	case CameraType::ORTHOGRAPHIC:
	{
		SetOrthographic(m_Size);
		break;
	}
	case CameraType::PERSPECTIVE:
	{
		SetPerspective(m_Size);
		break;
	}
	default:
		break;
	}
}

void Camera::UpdateProjection()
{
	switch (m_Type)
	{
	case CameraType::PERSPECTIVE:
	{
		SetPerspective(m_Size);
		break;
	}
	case CameraType::ORTHOGRAPHIC:
	default:
	{
		SetOrthographic(m_Size);
		break;
	}
	}
}

void Camera::Movement()
{
	if (Input::Mouse::IsMouseReleased(Keycode::MOUSE_BUTTON_2))
	{
		Window::GetInstance().ShowCursor();
		m_IsMoving = false;
	}

	switch (m_Type)
	{
	case CameraType::PERSPECTIVE:
	{
		MovePerspective();
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
		
		UpdateProjection();
	}
}

void Camera::SetFov(float fov)
{
	m_Fov = fov;

	UpdateProjection();
}

void Camera::SetZNear(float zNear)
{
	m_Znear = zNear;

	UpdateProjection();
}

void Camera::SetZFar(float zFar)
{
	m_Zfar = zFar;

	UpdateProjection();
}
