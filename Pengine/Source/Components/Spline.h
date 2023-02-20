#pragma once

#include "../Core/Core.h"
#include "../Core/Component.h"
#include "../EventSystem/Listener.h"

#include <vector>

namespace Pengine
{

	/**
	 * @Deprectated. Used for tests.
	 */
	class PENGINE_API Spline : public IComponent, public IListener
	{
	protected:

		virtual void OnStart() override;

		virtual void OnUpdate() override;

		virtual void Copy(const IComponent& component) override;

		virtual void Delete() override;

		virtual IComponent* New(GameObject* owner) override;

		virtual void OnRegisterClient() override;
	public:

		std::vector<class GameObject*> m_Points;
		std::vector<float> m_Lengths;

		float m_Speed = 1.0f;
		bool m_Visualize = false;

		static IComponent* Create(GameObject* owner);

		Spline() = default;

		~Spline();

		float GetLength() const;

		float GetNormalizedOffset(float& t) const;

		glm::vec3 GetPoint(float t) const;

		void CalculateLengths();

		void Visualize();
	};

}