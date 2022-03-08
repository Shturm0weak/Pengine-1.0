#pragma once

#include "../Core/Core.h"
#include "../Core/Component.h"
#include "../Core/Allocator.h"
#include "../OpenGL/Shader.h"
#include "Box2D/include/box2d/b2_body.h"

namespace Pengine
{

	class PENGINE_API Rigidbody2D : public IComponent, public IAllocator
	{
	public:

		enum class BodyType { Static = 0, Dynamic, Kinematic };

		BodyType m_BodyType = BodyType::Static;
		b2Fixture* m_Fixture = nullptr;
		b2Body* m_Body = nullptr;
		bool m_FixedRotation = false;

		static IComponent* Create(GameObject* owner);
		static b2BodyType ConvertToB2BodyType(BodyType bodyType);

		virtual void Copy(const IComponent& component) override {}
		virtual IComponent* Rigidbody2D::CreateCopy(GameObject* newOwner) override;
		virtual void Delete() override;

		void operator=(const Rigidbody2D& rb2d);
		void Initialize();

		Rigidbody2D() = default;

		friend class Scene;
	};

}
