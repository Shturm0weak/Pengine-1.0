#pragma once

#include "../Core/Core.h"
#include "../Core/Component.h"
#include "../EventSystem/Listener.h"

#include <vector>

namespace Pengine
{

	class PENGINE_API Spline : public IComponent, public IListener
	{
	public:

		std::vector<class GameObject*> m_Points;
		std::vector<float> m_Lengths;

		float m_Speed = 1.0f;
		bool m_Visualize = false;

		virtual IComponent* New(GameObject* owner) override;

		virtual void Copy(const IComponent& component) override;

		virtual void Delete() override;

		virtual void OnRegisterClient() override;

		Spline() = default;
		~Spline();

		static IComponent* Create(GameObject* owner);

		virtual void OnStart() override;

		virtual void OnUpdate() override;

		float GetLength() const;

		float GetNormalizedOffset(float& t) const;

		glm::vec3 GetPoint(float t) const;

		void CalculateLengths();

		void Visualize();
	};

}