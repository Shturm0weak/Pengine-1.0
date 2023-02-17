#include "Transform.h"

#include "../Core/Utils.h"
#include "../Core/GameObject.h"

using namespace Pengine;

void Transform::Copy(const IComponent& component)
{
	if (!m_Copyable) return;

	Transform& transform = *(Transform*)&component;
	Translate(transform.GetPosition(System::LOCAL));
	Rotate(transform.GetRotation(System::LOCAL));
	Scale(transform.GetScale(System::LOCAL));
	m_FollowOwner = transform.m_FollowOwner;
	m_Type = transform.m_Type;
}

IComponent* Transform::New(GameObject* owner)
{
	return new Transform();
}

IComponent* Transform::CreateCopy(GameObject* newOwner)
{
	return new Transform(*this);
}

void Transform::CopyGlobal(const Transform& transform)
{
	if (!m_Copyable) return;

	Translate(transform.GetPosition());
	Rotate(transform.GetRotation());
	Scale(transform.GetScale());
	m_FollowOwner = transform.m_FollowOwner;
	m_Type = transform.m_Type;
}

glm::mat4 Transform::GetPositionMat4(System system, bool isTransformed) const
{
	switch (system)
	{
	case System::LOCAL:
	{
		return m_PositionMat4;
		break;
	}
	case System::GLOBAL:
	{
		glm::mat4 positionMat4 = m_PositionMat4;
		if (Transform* parent = m_Parent)
		{
			positionMat4 = (isTransformed ? parent->GetTransform() : parent->GetPositionMat4()) * positionMat4;
		}
		return positionMat4;
		break;
	}
	default:
		return glm::mat4();
		break;
	}
}

glm::mat4 Transform::GetRotationMat4(System system, bool isTransformed) const
{
	switch (system)
	{
	case System::LOCAL:
	{
		return m_RotationMat4;
		break;
	}
	case System::GLOBAL:
	{
		glm::mat4 rotationMat4 = m_RotationMat4;
		if (Transform* parent = m_Parent)
		{
			rotationMat4 = (isTransformed ? parent->GetRotationMat4() * parent->GetScaleMat4() : parent->GetRotationMat4()) * rotationMat4;
		}
		return rotationMat4;
		break;
	}
	default:
		return glm::mat4();
		break;
	}
}

glm::mat4 Transform::GetScaleMat4(System system, bool isTransformed) const
{
	switch (system)
	{
	case System::LOCAL:
	{
		return m_ScaleMat4;
		break;
	}
	case System::GLOBAL:
	{
		glm::mat4 scaleMat4 = m_ScaleMat4;
		if (Transform* parent = m_Parent)
		{
			scaleMat4 = parent->GetScaleMat4() * scaleMat4;
		}
		return scaleMat4;
		break;
	}
	default:
		return glm::mat4();
		break;
	}
}

glm::vec3 Transform::GetPreviousPosition(System system) const
{
	switch (system)
	{
	case System::LOCAL:
	{
		return m_PreviousPosition;
		break;
	}
	case System::GLOBAL:
	{
		glm::vec3 previousPosition = m_PreviousPosition;
		if (Transform* parent = m_Parent)
		{
			previousPosition = parent->GetTransform() * glm::vec4(previousPosition, 1.0f);
		}
		return previousPosition;
		break;
	}
	default:
		return glm::vec3();
		break;
	}
}

glm::vec3 Transform::GetPositionDelta(System system) const
{
	switch (system)
	{
	case System::LOCAL:
	{
		return m_PositionDelta;
		break;
	}
	case System::GLOBAL:
	{
		glm::vec3 positionDelta = m_PositionDelta;
		if (Transform* parent = m_Parent)
		{
			positionDelta = parent->GetTransform() * glm::vec4(positionDelta, 1.0f);
		}
		return positionDelta;
		break;
	}
	default:
		return glm::vec3();
		break;
	}
}

glm::vec3 Transform::GetPosition(System system) const
{
	switch (system)
	{
	case System::LOCAL:
	{
		return Utils::GetPosition(m_PositionMat4);
		break;
	}
	case System::GLOBAL:
	{
		return Utils::GetPosition(GetPositionMat4(system));
		break;
	}
	default:
		return glm::vec3();
		break;
	}
}

glm::vec3 Transform::GetRotation(System system) const
{
	switch (system)
	{
	case System::LOCAL:
	{
		return m_Rotation;
		break;
	}
	case System::GLOBAL:
	{
		glm::vec3 rotation = m_Rotation;
		if (Transform* parent = m_Parent)
		{
			rotation += parent->GetRotation();
		}
		return rotation;
		break;
	}
	default:
		return glm::vec3();
		break;
	}
}

glm::vec3 Transform::GetScale(System system) const
{
	switch (system)
	{
	case System::LOCAL:
	{
		return Utils::GetScale(m_ScaleMat4);
		break;
	}
	case System::GLOBAL:
	{
		return Utils::GetScale(GetScaleMat4(system));
		break;
	}
	default:
		return glm::vec3();
		break;
	}
}

glm::mat4 Transform::GetTransform(System system) const
{
	switch (system)
	{
	case System::LOCAL:
	{
		return m_TransformMat4;
		break;
	}
	case System::GLOBAL:
	{
		glm::mat4 transformMat4 = m_TransformMat4;
		if (Transform* parent = m_Parent)
		{
			transformMat4 = parent->GetTransform() * transformMat4;
		}
		return transformMat4;
		break;
	}
	default:
		return glm::mat4();
		break;
	}
}

glm::mat3 Transform::GetInverseTransform(System system) const
{
	switch (system)
	{
	case System::LOCAL:
	{
		return m_InverseTransformMat3;
		break;
	}
	case System::GLOBAL:
	{
		glm::mat3 inverseTransformMat3 = m_InverseTransformMat3;
		if (Transform* parent = m_Parent)
		{
			inverseTransformMat3 = parent->GetInverseTransform() * inverseTransformMat3;
		}
		return inverseTransformMat3;
		break;
	}
	default:
		return glm::mat3();
		break;
	}
}

void Transform::Move(Transform&& transform) noexcept
{
	m_Rotation = std::move(transform.m_Rotation);
	m_PositionMat4 = std::move(transform.m_PositionMat4);
	m_ScaleMat4 = std::move(transform.m_ScaleMat4);
	m_RotationMat4 = std::move(transform.m_RotationMat4);
	m_FollowOwner = transform.m_FollowOwner;
}

void Transform::UpdateVectors()
{
	const float cosPitch = cos(m_Rotation.x);
	m_Back.z = cos(m_Rotation.y) * cosPitch;
	m_Back.x = sin(m_Rotation.y) * cosPitch;
	m_Back.y = sin(m_Rotation.x);
	m_Back = glm::normalize(m_Back);

	const glm::mat3 inverseTransform = m_InverseTransformMat3;
	m_Up = glm::normalize(glm::vec3(inverseTransform[0][1],
		inverseTransform[1][1], inverseTransform[2][1]));
}

void Transform::UpdateTransforms()
{
	m_TransformMat4 = m_PositionMat4 * m_RotationMat4 * m_ScaleMat4;
	m_InverseTransformMat3 = glm::inverse(m_TransformMat4);
}

void Transform::operator=(const Transform& transform)
{
	Copy(transform);
}

void Transform::operator=(Transform&& transform) noexcept
{
	Move(std::move(transform));
}

Transform::Transform(const Transform& transform)
{
	Copy(transform);
}

Transform::Transform(Transform&& transform) noexcept
	: m_Rotation(std::move(transform.m_Rotation))
	, m_PositionMat4(std::move(transform.m_PositionMat4))
	, m_ScaleMat4(std::move(transform.m_ScaleMat4))
	, m_RotationMat4(std::move(transform.m_RotationMat4))
	, m_FollowOwner(transform.m_FollowOwner)
{
	m_Type = "Pengine::Transform";
}

Transform::Transform(const glm::vec3& position, const glm::vec3& scale, const glm::vec3& rotation)
{
	Translate(position);
	Rotate(rotation);
	Scale(scale);
	m_Type = "Pengine::Transform";
}

void Transform::RemoveOnRotationCallback(const std::string& label)
{
	auto callback = m_OnRotationCallbacks.find(label);
	if (callback != m_OnRotationCallbacks.end())
	{
		m_OnRotationCallbacks.erase(callback);
	}
}

void Transform::RemoveOnTranslationCallback(const std::string& label)
{
	auto callback = m_OnTranslationCallbacks.find(label);
	if (callback != m_OnTranslationCallbacks.end())
	{
		m_OnTranslationCallbacks.erase(callback);
	}
}

void Transform::RemoveOnScaleCallback(const std::string& label)
{
	auto callback = m_OnScaleCallbacks.find(label);
	if (callback != m_OnScaleCallbacks.end())
	{
		m_OnScaleCallbacks.erase(callback);
	}
}

void Transform::SetRotationMat4(const glm::mat4& rotationMat4)
{
	m_RotationMat4 = rotationMat4;

	UpdateTransforms();
	UpdateVectors();

	for (auto& onRotationCallback : m_OnRotationCallbacks)
	{
		onRotationCallback.second();
	}
}

void Transform::Translate(const glm::vec3& position)
{
	m_PreviousPosition = GetPosition(System::LOCAL);
	m_PositionDelta = position - m_PreviousPosition;
	m_PositionMat4 = glm::translate(glm::mat4(1.0f), position);

	UpdateTransforms();

	for (auto& onTranslationCallback : m_OnTranslationCallbacks)
	{
		onTranslationCallback.second();
	}
}

void Transform::Rotate(const glm::vec3& rotation)
{
	m_PreviousRotation = GetRotation(System::LOCAL);
	m_Rotation = rotation;
	m_RotationDelta = m_Rotation - m_PreviousRotation;
	m_RotationMat4 = glm::toMat4(glm::quat(m_Rotation));

	UpdateTransforms();
	UpdateVectors();

	for (auto& onRotationCallback : m_OnRotationCallbacks)
	{
		onRotationCallback.second();
	}
}

void Transform::Scale(const glm::vec3& scale)
{
	m_PreviousScale = GetScale(System::LOCAL);
	m_ScaleDelta = scale - m_PreviousScale;
	m_ScaleMat4 = glm::scale(glm::mat4(1.0f), scale);

	UpdateTransforms();

	for (auto& onScaleCallback : m_OnScaleCallbacks)
	{
		onScaleCallback.second();
	}
}

void Transform::LogTransform()
{
	const glm::vec3 position = GetPosition();
	const glm::vec3 rotation = GetRotation();
	const glm::vec3 scale = GetScale();
	printf_s("Transform\n");
	printf_s("Position X : %.3f Y : %.3f Z : %.3f\n", position.x, position.y, position.z);
	printf_s("Rotation X: %.3f Y: %.3f Z: %.3f\n", rotation.x, rotation.y, rotation.z);
	printf_s("Scale X: %.3f Y: %.3f Z: %.3f\n", scale.x, scale.y, scale.z);
}
