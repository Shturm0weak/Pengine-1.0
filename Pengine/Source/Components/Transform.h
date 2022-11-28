#pragma once

#include "../Core/Core.h"
#include "../Core/Component.h"

#include <functional>
#include <unordered_map>

namespace Pengine
{

	class PENGINE_API Transform : public IComponent
	{
	private:

		glm::mat4 m_TransformMat4;
		glm::mat4 m_PositionMat4;
		glm::mat4 m_RotationMat4;
		glm::mat4 m_ScaleMat4;
		glm::mat3 m_InverseTransformMat3;
		glm::vec3 m_PreviousPosition;
		glm::vec3 m_PositionDelta;
		glm::vec3 m_PreviousScale;
		glm::vec3 m_ScaleDelta;
		glm::vec3 m_Rotation;
		glm::vec3 m_PreviousRotation;
		glm::vec3 m_RotationDelta;
		glm::vec3 m_Back;
		glm::vec3 m_Up;

		std::unordered_map<std::string, std::function<void()>> m_OnRotationCallbacks;
		std::unordered_map<std::string, std::function<void()>> m_OnTranslationCallbacks;
		std::unordered_map<std::string, std::function<void()>> m_OnScaleCallbacks;

		bool m_FollowOwner = true;
		bool m_Copyable = true;

		void Move(Transform&& transform) noexcept;
		void UpdateVectors();
		void UpdateTransforms();

		friend class GameObject;
		friend class Editor;
	public:

		Transform(const Transform& transform);
		Transform(Transform&& transform) noexcept;
		Transform(
			const glm::vec3& position = glm::vec3(0.0f),
			const glm::vec3& scale = glm::vec3(1.0f),
			const glm::vec3& rotation = glm::vec3(0.0f)
		);
		void operator=(const Transform& transform);
		void operator=(Transform&& transform) noexcept;

		virtual void Copy(const IComponent& component) override;
		
		virtual IComponent* New(GameObject* owner) override;
		
		virtual IComponent* CreateCopy(GameObject* newOwner) override;
		
		glm::mat4 GetPositionMat4() const { return m_PositionMat4; }
		
		glm::mat4 GetRotationMat4() const { return m_RotationMat4; }
		
		glm::mat4 GetScaleMat4() const { return m_ScaleMat4; }
		
		glm::vec3 GetPreviousPosition() const { return m_PreviousPosition; }
		
		glm::vec3 GetPositionDelta() const { return m_PositionDelta; }
		
		glm::vec3 GetPosition() const;
		
		glm::vec3 GetRotation() const { return m_Rotation; }
		
		glm::vec3 GetScale() const;
		
		glm::vec3 GetBack() const { return m_Back; }
		
		glm::vec3 GetUp() const { return m_Up; }
		
		glm::vec3 GetForward() const { return glm::normalize(glm::vec3(-m_Back.x, m_Back.y, -m_Back.z)); }
		
		glm::vec3 GetRight() const { return glm::normalize(glm::cross(GetForward(), GetUp())); }
		
		glm::mat4 GetTransform() const { return m_TransformMat4; }

		glm::mat3 GetInverseTransform() const { return m_InverseTransformMat3; }
		
		bool GetFollorOwner() const { return m_FollowOwner; }
		
		void SetFollowOwner(bool followOwner) { m_FollowOwner = followOwner; }

		bool IsCopyable() const { return m_Copyable; }

		void SetCopyable(bool copyable) { m_Copyable = copyable; }
		
		void SetOnRotationCallback(const std::string& label, std::function<void()> callback) { m_OnRotationCallbacks.emplace(label, callback); }
		
		void SetOnTranslationCallback(const std::string& label, std::function<void()> callback) { m_OnTranslationCallbacks.emplace(label, callback); }

		void SetOnScaleCallback(const std::string& label, std::function<void()> callback) { m_OnScaleCallbacks.emplace(label, callback); }

		void RemoveOnRotationCallback(const std::string& label);

		void RemoveOnTranslationCallback(const std::string& label);

		void RemoveOnScaleCallback(const std::string& label);

		void ClearOnRotationCallbacks() { m_OnRotationCallbacks.clear(); }

		void ClearOnTranslationCallbacks() { m_OnTranslationCallbacks.clear(); }
		
		void ClearOnScaleCallbacks() { m_OnScaleCallbacks.clear(); }
		
		void SetRotationMat4(const glm::mat4& rotationMat4);

		void Translate(const glm::vec3& position);
		
		void Rotate(const glm::vec3& rotation);
		
		void Scale(const glm::vec3& scale);

		void LogTransform();
	};

}