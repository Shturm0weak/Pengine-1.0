#include "PointLight2D.h"

#include "../Core/Scene.h"

using namespace Pengine;

void PointLight2D::Delete()
{
	Utils::Erase(m_Owner->GetScene()->m_PointLights2D, this);
}

IComponent* PointLight2D::New(GameObject* owner)
{
	return Create(owner);
}

void PointLight2D::Copy(const IComponent& component)
{
	PointLight2D& pointLight2D = *(PointLight2D*)&component;
	m_Type = component.GetType();

	m_Color = pointLight2D.m_Color;
	m_Constant = pointLight2D.m_Constant;
	m_Linear = pointLight2D.m_Linear;
	m_Quadratic = pointLight2D.m_Quadratic;
}

IComponent* PointLight2D::Create(GameObject* owner)
{
	PointLight2D* pointLight2D = new PointLight2D();

	owner->GetScene()->m_PointLights2D.push_back(pointLight2D);

	return pointLight2D;
}
