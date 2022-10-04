#include "DirectionalLight.h"

#include "../Core/Scene.h"

using namespace Pengine;

IComponent* DirectionalLight::New(GameObject* owner)
{
	return Create(owner);
}
void DirectionalLight::Copy(const IComponent& component)
{
}

void DirectionalLight::Delete()
{
	Utils::Erase(m_Owner->GetScene()->m_DirectionalLight, this);
}

IComponent* DirectionalLight::Create(GameObject* owner)
{
	DirectionalLight* directionalLight = new DirectionalLight();

	owner->GetScene()->m_DirectionalLight.push_back(directionalLight);

	return directionalLight;
}
