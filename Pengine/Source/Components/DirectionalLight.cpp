#include "DirectionalLight.h"

#include "../Core/Scene.h"

using namespace Pengine;

IComponent* DirectionalLight::New(GameObject* owner)
{
	return Create(owner);
}

void DirectionalLight::Copy(const IComponent& component)
{
	DirectionalLight& directionalLight = *(DirectionalLight*)&component;
	m_Type = component.GetType();

	m_Intensity = directionalLight.m_Intensity;
	m_Color = directionalLight.m_Color;
}

void DirectionalLight::Delete()
{
	Utils::Erase(m_Owner->GetScene()->m_DirectionalLights, this);
	delete this;
}

IComponent* DirectionalLight::Create(GameObject* owner)
{
	DirectionalLight* directionalLight = new DirectionalLight();

	auto onRotationCallback = [owner, directionalLight]()
	{
		directionalLight->SetDirection(owner->m_Transform.GetRotationMat4() * glm::vec4(0, 0, -1, 1));
	};

	onRotationCallback();
	owner->m_Transform.SetOnRotationCallback(onRotationCallback);

	owner->GetScene()->m_DirectionalLights.push_back(directionalLight);

	return directionalLight;
}
