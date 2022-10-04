#pragma once

#include "../Core/Core.h"
#include "../Core/TextureManager.h"

namespace Pengine
{
	struct PENGINE_API Material
	{
		glm::vec3 m_Ambient = { 1.0f, 1.0f, 1.0f };
		glm::vec3 m_Diffuse = { 1.0f, 1.0f, 1.0f };
		glm::vec3 m_Specular = { 1.0f, 1.0f, 1.0f };
		float m_Shininess = 32.0f;
		float m_Scale = 1.0f;
		float m_Solid = 1.0f;
		Texture* m_BaseColor = TextureManager::GetInstance().White();

		void operator=(const Material& material);
	};
	
}