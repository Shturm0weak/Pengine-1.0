#include "PointLight.h"

#include "../Core/Scene.h"
#include "../Core/Renderer.h"
#include "../OpenGL/FrameBuffer.h"

using namespace Pengine;

void PointLight::Delete()
{
	Utils::Erase(m_Owner->GetScene()->m_PointLights, this);
	delete m_ShadowsCubeMap;
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
	m_Fog = pointLight.m_Fog;
	m_ZFar = pointLight.m_ZFar;
	m_ZNear = pointLight.m_ZNear;
	m_DrawShadows = pointLight.m_DrawShadows;
}

IComponent* PointLight::Create(GameObject* owner)
{
	PointLight* pointLight = new PointLight();

	owner->GetScene()->m_PointLights.push_back(pointLight);

	glm::ivec2 shadowCubeMapSize = { 1024, 1024 };
	pointLight->m_ShadowsCubeMap = new FrameBuffer(
		{
			{ shadowCubeMapSize, GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT, true, true }
		},
		{
			{ GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST },
			{ GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST },
			{ GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE },
			{ GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE },
			{ GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE }
		}
	);

	pointLight->m_ShadowsTransforms.resize(6);
	auto onTranslationCallback = [owner, pointLight, shadowCubeMapSize]()
	{
		glm::vec3 position = owner->m_Transform.GetPosition();
		glm::mat4 projection = glm::perspective(glm::radians(90.0f), (float)shadowCubeMapSize.x / (float)shadowCubeMapSize.y, pointLight->m_ZNear, pointLight->m_ZFar);
		pointLight->m_ShadowsTransforms[0] = (projection * glm::lookAt(position, position + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		pointLight->m_ShadowsTransforms[1] = (projection * glm::lookAt(position, position + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		pointLight->m_ShadowsTransforms[2] = (projection * glm::lookAt(position, position + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
		pointLight->m_ShadowsTransforms[3] = (projection * glm::lookAt(position, position + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
		pointLight->m_ShadowsTransforms[4] = (projection * glm::lookAt(position, position + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		pointLight->m_ShadowsTransforms[5] = (projection * glm::lookAt(position, position + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	};

	onTranslationCallback();
	owner->m_Transform.SetOnTranslationCallback(onTranslationCallback);

	return pointLight;
}
