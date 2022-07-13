#pragma once

#include "../Core/Core.h"
#include "../Core/Component.h"

#include <functional>

namespace Pengine
{

	class PENGINE_API Transform : public IComponent
	{
	private:

		glm::mat4 m_PositionMat4;
		glm::mat4 m_RotationMat4;
		glm::mat4 m_ScaleMat4;
		glm::vec3 m_PreviousPosition;
		glm::vec3 m_PositionDelta;
		glm::vec3 m_Rotation;
		glm::vec3 m_Back;

		std::function<void()> m_OnRotationCallback;
		std::function<void()> m_OnTranslationCallback;

		bool m_FollowOwner = true;

		void Move(Transform&& transform) noexcept;
		void UpdateBack();

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
		
		glm::vec3 GetPreviousPosition() const;
		
		glm::vec3 GetPositionDelta() const;
		
		glm::vec3 GetPosition() const;
		
		glm::vec3 GetRotation() const;
		
		glm::vec3 GetScale() const;
		
		glm::vec3 GetBack() const { return m_Back; };
		
		glm::vec3 GetUp() const { return glm::normalize(glm::vec3(m_RotationMat4[0][1], m_RotationMat4[1][1], m_RotationMat4[2][1])); };
		
		glm::vec3 GetForward() const { return glm::normalize(glm::vec3(-m_Back.x, m_Back.y, -m_Back.z)); };
		
		glm::vec3 GetRight() const { return glm::normalize(glm::cross(GetForward(), GetUp())); };
		
		glm::mat4 GetTransform() const;
		
		bool GetFollorOwner() const { return m_FollowOwner; }
		
		void SetFollowOwner(bool followOwner) { m_FollowOwner = followOwner; }
		
		void SetOnRotationCallback(std::function<void()> callback) { m_OnRotationCallback = callback; }
		
		void SetOnTranslationCallback(std::function<void()> callback) { m_OnTranslationCallback = callback; }
		
		void Translate(const glm::vec3& position);
		
		void Rotate(const glm::vec3& rotation);
		
		void Scale(const glm::vec3& scale);

		void LogTransform();
	};

}