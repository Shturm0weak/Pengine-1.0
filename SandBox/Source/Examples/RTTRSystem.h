#pragma once

#include "Core/Core.h"
#include "Core/Component.h"
#include "Core/ReflectionSystem.h"

using namespace Pengine;

class ExampleComponent : public IComponent
{
	REGISTER_PARENT_CLASS(IComponent)
public: PROPERTY(GameObject*, m_GameObject, false)
public: PROPERTY(float, m_Float, 0.0f)
public: PROPERTY(double, m_Double, 0.0)
public: PROPERTY(int, m_Int, 0)
public: PROPERTY(bool, m_Bool, false)
public: PROPERTY(std::string, m_String, "")
public: PROPERTY(glm::vec2, m_Vec2, {})
public: PROPERTY(glm::vec3, m_Vec3, {})
public: PROPERTY(glm::vec4, m_Vec4, {})
public: PROPERTY(glm::ivec2, m_IVec2, {})
public: PROPERTY(glm::ivec3, m_IVec3, {})
public: PROPERTY(glm::ivec4, m_IVec4, {})
public:

	virtual IComponent* New(GameObject* owner) override
	{
		return Create(owner);
	}

	virtual void Copy(const IComponent& component) override
	{
		ExampleComponent& exampleComponent = *(ExampleComponent*)&component;
		m_Type = component.GetType();
	}

	ExampleComponent() = default;
	~ExampleComponent() = default;

	static IComponent* Create(GameObject* owner)
	{
		ExampleComponent* exampleComponent = new ExampleComponent();
		return exampleComponent;
	}
};
REGISTER_COMPONENT(ExampleComponent)

class ExampleVectorComponent : public ExampleComponent
{
	REGISTER_PARENT_CLASS(ExampleComponent)
public: PROPERTY(std::vector<float>, m_VectorFloat, {})
public: PROPERTY(std::vector<double>, m_VectorDouble, {})
public: PROPERTY(std::vector<int>, m_VectorInt, {})
public: PROPERTY(std::vector<bool>, m_VectorBool, {})
public: PROPERTY(std::vector<std::string>, m_VectorString, {})
public: PROPERTY(std::vector<glm::vec2>, m_VectorVec2, {})
public: PROPERTY(std::vector<glm::vec3>, m_VectorVec3, {})
public: PROPERTY(std::vector<glm::vec4>, m_VectorVec4, {})
public: PROPERTY(std::vector<glm::ivec2>, m_VectorIVec2, {})
public: PROPERTY(std::vector<glm::ivec3>, m_VectorIVec3, {})
public: PROPERTY(std::vector<glm::ivec4>, m_VectorIVec4, {})
public:

	virtual IComponent* New(GameObject* owner) override
	{
		return Create(owner);
	}

	virtual void Copy(const IComponent& component) override
	{
		ExampleVectorComponent& exampleVectorComponent = *(ExampleVectorComponent*)&component;
		m_Type = component.GetType();
	}

	ExampleVectorComponent() = default;
	~ExampleVectorComponent() = default;

	static IComponent* Create(GameObject* owner)
	{
		ExampleVectorComponent* exampleVectorComponent = new ExampleVectorComponent();
		return exampleVectorComponent;
	}
};
REGISTER_COMPONENT(ExampleVectorComponent)