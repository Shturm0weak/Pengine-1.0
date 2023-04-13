#include "PointLight.h"

#include "../Core/Scene.h"
#include "../OpenGL/FrameBuffer.h"

using namespace Pengine;

void PointLight::Copy(const IComponent& component)
{
	PointLight& pointLight = *(PointLight*)&component;
	m_Type = component.GetType();

	m_Color = pointLight.m_Color;
	m_Constant = pointLight.m_Constant;
	m_Linear = pointLight.m_Linear;
	m_Quadratic = pointLight.m_Quadratic;
	m_Fog = pointLight.m_Fog;
	m_ZFar = pointLight.m_ZFar;
	m_ZNear = pointLight.m_ZNear;
	m_DrawShadows = pointLight.m_DrawShadows;
	m_ShadowBias = pointLight.m_ShadowBias;
}

void PointLight::Delete()
{
	GetOwner()->m_Transform.RemoveOnTranslationCallback("PointLight");

	Utils::Erase(m_Owner->GetScene()->m_PointLights, this);
	delete this;
}

IComponent* PointLight::New(GameObject* owner)
{
	return Create(owner);
}

IComponent* PointLight::Create(GameObject* owner)
{
	PointLight* pointLight = new PointLight();

	owner->GetScene()->m_PointLights.push_back(pointLight);
	pointLight->m_Owner = owner;
	pointLight->SetDrawShadows(true);

	return pointLight;
}

void PointLight::SetDrawShadows(bool drawShadows)
{
	if (m_DrawShadows == drawShadows) return;

	m_DrawShadows = drawShadows;

	if (m_DrawShadows)
	{
		glm::ivec2 shadowCubeMapSize = { 1024, 1024 };
		m_ShadowsCubeMap = std::make_shared<FrameBuffer>(
			shadowCubeMapSize,
			std::vector<FrameBuffer::FrameBufferParams>
			{
				{ GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT, true, true }
			},
			std::vector<Texture::TexParameteri>
			{
				{ GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST },
				{ GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST },
				{ GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE },
				{ GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE },
				{ GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE }
			}
		);

		auto onTranslationCallback = [this, shadowCubeMapSize]()
		{
			BuildProjectionMatrix();
		};

		onTranslationCallback();
		GetOwner()->m_Transform.ClearOnTranslationCallbacks();
		GetOwner()->m_Transform.SetOnTranslationCallback("PointLight", onTranslationCallback);
	}
	else
	{
		GetOwner()->m_Transform.ClearOnTranslationCallbacks();
		m_ShadowsCubeMap.reset();
	}
}

bool PointLight::IsRenderShadows() const
{
	return m_DrawShadows && m_ShadowsVisible && GetOwner()->IsEnabled();
}

void PointLight::BuildProjectionMatrix()
{
	if (!GetOwner() || !m_ShadowsCubeMap)
	{
		return;
	}

	m_ShadowsTransforms.resize(6);
	glm::vec3 position = GetOwner()->m_Transform.GetPosition();
	glm::mat4 projection = glm::perspective(glm::radians(90.0f), (float)m_ShadowsCubeMap->m_Size.x / (float)m_ShadowsCubeMap->m_Size.y, m_ZNear, m_ZFar);
	m_ShadowsTransforms[0] = (projection * glm::lookAt(position, position + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	m_ShadowsTransforms[1] = (projection * glm::lookAt(position, position + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	m_ShadowsTransforms[2] = (projection * glm::lookAt(position, position + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
	m_ShadowsTransforms[3] = (projection * glm::lookAt(position, position + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
	m_ShadowsTransforms[4] = (projection * glm::lookAt(position, position + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	m_ShadowsTransforms[5] = (projection * glm::lookAt(position, position + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
}
