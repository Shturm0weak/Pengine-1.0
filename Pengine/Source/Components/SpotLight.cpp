#include "SpotLight.h"

#include "../Core/Scene.h"
#include "../Core/MemoryManager.h"
#include "../OpenGL/FrameBuffer.h"

using namespace Pengine;

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
	m_ShadowBias = spotLight.m_ShadowBias;
	m_ShadowPcf = spotLight.m_ShadowPcf;
}

void SpotLight::Delete()
{
	GetOwner()->m_Transform.RemoveOnRotationCallback("SpotLight");

	Utils::Erase(m_Owner->GetScene()->m_SpotLights, this);
	MemoryManager::GetInstance().FreeMemory<SpotLight>(static_cast<IAllocator*>(this));
}

IComponent* SpotLight::New(GameObject* owner)
{
	return Create(owner);
}

IComponent* SpotLight::Create(GameObject* owner)
{
	SpotLight* spotLight = MemoryManager::GetInstance().Allocate<SpotLight>();

	auto onRotationCallback = [owner, spotLight]()
	{
		spotLight->SetDirection(owner->m_Transform.GetRotationMat4() * glm::vec4(0.0f, 0.0f, -1.0f, 1.0f));
		spotLight->BuildProjectionMatrix();
	};

	onRotationCallback();
	owner->m_Transform.SetOnRotationCallback("SpotLight", onRotationCallback);

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

void SpotLight::SetDrawShadows(bool drawShadows)
{
	if (m_DrawShadows == drawShadows) return;

	m_DrawShadows = drawShadows;

	if (m_DrawShadows)
	{
		glm::ivec2 shadowMapSize = { 1024, 1024 };
		m_ShadowMap = std::make_shared<FrameBuffer>(
			std::vector<FrameBuffer::FrameBufferParams>
		{
			{ shadowMapSize, GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT, false, true }
		},
			std::vector<Texture::TexParameteri>
		{
			{ GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST },
			{ GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST },
			{ GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER },
			{ GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER },
		}
		);

		auto onTranslationCallback = [this, shadowMapSize]()
		{
			BuildProjectionMatrix();
		};

		onTranslationCallback();
		GetOwner()->m_Transform.ClearOnTranslationCallbacks();
		GetOwner()->m_Transform.SetOnTranslationCallback("SpotLight", onTranslationCallback);
	}
	else
	{
		GetOwner()->m_Transform.ClearOnTranslationCallbacks();
		m_ShadowMap.reset();
	}
}

bool SpotLight::IsRenderShadows() const
{
	return m_DrawShadows && m_ShadowsVisible && GetOwner()->IsEnabled();
}

void SpotLight::BuildProjectionMatrix()
{
	if (!GetOwner() || !m_ShadowMap)
	{
		return;
	}

	glm::vec3 position = GetOwner()->m_Transform.GetPosition();
	glm::mat4 projection = glm::perspective(glm::radians(90.0f), (float)m_ShadowMap->m_Params[0].m_Size.x / (float)m_ShadowMap->m_Params[0].m_Size.y, m_ZNear, m_ZFar);
	m_ShadowTransform = projection * glm::lookAt(position, position + GetDirection(), GetOwner()->m_Transform.GetUp());
}
