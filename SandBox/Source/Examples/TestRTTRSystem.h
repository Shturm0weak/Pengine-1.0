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
	public: PROPERTY(Person, bool, m_Male, true)
	public: PROPERTY(Person, glm::vec2, m_Vec2, glm::vec2())
	public: PROPERTY(Person, glm::vec3, m_Vec3, glm::vec3())
	public: PROPERTY(Person, glm::vec4, m_Vec4, glm::vec4())
	public: PROPERTY(Person, glm::ivec2, m_Ivec2, glm::ivec2())
	public: PROPERTY(Person, glm::ivec3, m_Ivec3, glm::ivec3())
	public: PROPERTY(Person, glm::ivec4, m_Ivec4, glm::ivec4())
	public: PROPERTY(Person, std::vector<std::string>, m_VecStr, { "One" COM "Two"})
	public: PROPERTY(Person, std::vector<int>, m_VecInt, { 1 COM 2 COM 3})
	public: PROPERTY(Person, std::vector<float>, m_VecFloat, { 1.0f COM 2.0f COM 3.0f })
	public: PROPERTY(Person, std::vector<double>, m_VecDouble, { 1.0 COM 2.0 COM 3.0 })
	public: PROPERTY(Person, std::vector<bool>, m_VecBool, { true COM false COM true})
	public: PROPERTY(Person, std::vector<glm::vec2>, m_VecVec2, { glm::vec2(1.0f) COM glm::vec2(2.0f) COM glm::vec2(3.0f) })
	public: PROPERTY(Person, std::vector<glm::vec3>, m_VecVec3, { glm::vec3(1.0f) COM glm::vec3(2.0f) COM glm::vec3(3.0f) })
	public: PROPERTY(Person, std::vector<glm::vec4>, m_VecVec4, { glm::vec4(1.0f) COM glm::vec4(2.0f) COM glm::vec4(3.0f) })
	public: PROPERTY(Person, std::vector<glm::ivec2>, m_VecIVec2, { glm::ivec2(1) COM glm::ivec2(2) COM glm::ivec2(3) })
	public: PROPERTY(Person, std::vector<glm::ivec3>, m_VecIVec3, { glm::ivec3(1) COM glm::ivec3(2) COM glm::ivec3(3) })
	public: PROPERTY(Person, std::vector<glm::ivec4>, m_VecIVec4, { glm::ivec4(1) COM glm::ivec4(2) COM glm::ivec4(3) })
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