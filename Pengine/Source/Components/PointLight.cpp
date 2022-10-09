#include "PointLight.h"

#include "../Core/Scene.h"

using namespace Pengine;

void PointLight::Delete()
{
	Utils::Erase(m_Owner->GetScene()->m_PointLights, this);
	delete this;
}

IComponent* PointLight::New(GameObject* owner)
{
	return Create(owner);
}

void PointLight::Copy(const IComponent& component)
{
	PointLight& pointLight = *(PointLight*)&component;
	m_Type = component.GetType();

	m_Color = pointLight.m_Color;
	m_Constant = pointLight.m_Constant;
	m_Linear = pointLight.m_Linear;
	m_Quadratic = pointLight.m_Quadratic;
}

IComponent* PointLight::Create(GameObject* owner)
{
	PointLight* pointLight = new PointLight();

	owner->GetScene()->m_PointLights.push_back(pointLight);

	return pointLight;
}
