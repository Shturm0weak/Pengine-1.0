#pragma once

#include "Core/Core.h"
#include "Core/Application.h"
#include "Core/Component.h"
#include "Core/ReflectionSystem.h"

using namespace Pengine;

class Person : public IComponent
{
	RTTR_ENABLE(IComponent)

	public: PROPERTY(Person, std::string, m_Name, "Alexander")
	public: PROPERTY(Person, int, m_Age, 21)
	public: PROPERTY(Person, float, m_Height, 170.0f)
	public: PROPERTY(Person, double, m_Weight, 65.55)
	public: PROPERTY(Person, glm::vec2, m_Vec2, glm::vec2())
	public: PROPERTY(Person, glm::vec3, m_Vec3, glm::vec3())
	public: PROPERTY(Person, glm::vec4, m_Vec4, glm::vec4())
	public: PROPERTY(Person, Texture*, m_Texture, TextureManager::GetInstance().Get("White"))

public:

	virtual IComponent* New(GameObject* owner) override
	{
		return Create(owner);
	}

	virtual void Copy(const IComponent& component) override
	{
		Person& person = *(Person*)&component;
		m_Type = component.GetType();
	}

	Person() = default;
	~Person() = default;

	static IComponent* Create(GameObject* owner)
	{
		Person* person = new Person();
		return person;
	}
};
REGISTER_CLASS(Person)

class TestRTTRSystem : public Application
{
public:

	TestRTTRSystem(const std::string& title = "TestRTTRSystem")
		: Application(title)
	{}

	virtual void OnStart() override {};

	virtual void OnPostStart() override {};

	virtual void OnUpdate() override {};

	virtual void OnGuiRender() override {};

	virtual void OnImGuiRender() override {};

	virtual void OnClose() override {};
};