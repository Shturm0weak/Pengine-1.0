#include "SpotLight.h"

#include "../Core/Scene.h"
#include "../Core/MemoryManager.h"

using namespace Pengine;

void SpotLight::Delete()
{
	Utils::Erase(m_Owner->GetScene()->m_SpotLights, this);
	MemoryManager::GetInstance().FreeMemory<SpotLight>(static_cast<IAllocator*>(this));
}

IComponent* SpotLight::New(GameObject* owner)
{
	return Create(owner);
}

void SpotLight::Copy(const IComponent& component)
{
	SpotLight& spotLight = *(SpotLight*)&component;
	m_Type = component.GetType();

	m_Color = spotLight.m_Color;
	m_Constant = spotLight.m_Constant;
	m_Linear = spotLight.m_Linear;
	m_Quadratic = spotLight.m_Quadratic;
	m_InnerCutOff = spotLight.m_InnerCutOff;
	m_OuterCutOff = spotLight.m_OuterCutOff;
	m_CosInnerCutOff = spotLight.m_CosInnerCutOff;
	m_CosOuterCutOff = spotLight.m_CosOuterCutOff;
}

IComponent* SpotLight::Create(GameObject* owner)
{
	SpotLight* spotLight = MemoryManager::GetInstance().Allocate<SpotLight>();

	auto onRotationCallback = [owner, spotLight]()
	{
		spotLight->SetDirection(owner->m_Transform.GetRotationMat4() * glm::vec4(0, 0, -1, 1));
	};

	onRotationCallback();
	owner->m_Transform.SetOnRotationCallback(onRotationCallback);

	owner->GetScene()->m_SpotLights.push_back(spotLight);

	return spotLight;
}

void SpotLight::SetInnerCutOff(float innerCutOff)
{
	m_InnerCutOff = innerCutOff;
	m_CosInnerCutOff = glm::cos(innerCutOff);
}

void SpotLight::SetOuterCutOff(float outerCutOff)
{
	m_OuterCutOff = outerCutOff;
	m_CosOuterCutOff = glm::cos(outerCutOff);
}
