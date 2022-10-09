#include "Transform.h"

#include "../Core/Utils.h"
#include "../Core/GameObject.h"

using namespace Pengine;

void Transform::Copy(const IComponent& component)
{
	if (!m_Copyable) return;

	Transform& transform = *(Transform*)&component;
	Translate(transform.GetPosition());
	Rotate(transform.GetRotation());
	Scale(transform.GetScale());
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

	glm::mat4 inverseTransform = glm::inverse(GetTransform());
	m_Up = glm::normalize(glm::vec3(inverseTransform[0][1],
		inverseTransform[1][1], inverseTransform[2][1]));
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

glm::vec3 Transform::GetPreviousPosition() const
{
	return m_PreviousPosition;
}

glm::vec3 Transform::GetPositionDelta() const
{
	return m_PositionDelta;
}

glm::vec3 Transform::GetPosition() const
{
	return Utils::GetPosition(m_PositionMat4);
}

glm::vec3 Transform::GetRotation() const
{
	return m_Rotation;
}

glm::vec3 Transform::GetScale() const
{
	return Utils::GetScale(m_ScaleMat4);
}

glm::mat4 Transform::GetTransform() const
{
	return m_PositionMat4 * m_RotationMat4 * m_ScaleMat4;
}

void Transform::Translate(const glm::vec3& position)
{
	m_PreviousPosition = Utils::GetPosition(m_PositionMat4);
	m_PositionDelta = position - m_PreviousPosition;

	m_PositionMat4 = glm::translate(glm::mat4(1.0f), position);
	for (auto& onTranslationCallback : m_OnTranslationCallbacks)
	{
		onTranslationCallback();
	}

	if (m_Owner)
	{
		m_Owner->ForChilds([=](GameObject& child) {
			if (child.m_Transform.m_FollowOwner)
			{
				child.m_Transform.Translate(Utils::GetPosition(child.m_Transform.m_PositionMat4) + m_PositionDelta);
			}
		});
	}
}

void Transform::Rotate(const glm::vec3& rotation)
{
	m_Rotation = rotation;
	m_RotationMat4 = glm::toMat4(glm::quat(m_Rotation));
	UpdateVectors();
	for (auto& onRotationCallback : m_OnRotationCallbacks)
	{
		onRotationCallback();
	}
}

void Transform::Scale(const glm::vec3& scale)
{
	m_ScaleMat4 = glm::scale(glm::mat4(1.0f), scale);
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
