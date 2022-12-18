#pragma once

#include "../Core/Core.h"
#include "../Core/TextureManager.h"
#include "../Core/Asset.h"

namespace Pengine
{

	class PENGINE_API Material : public IAsset
	{
	public:
		std::string m_ShaderFilePath = "Instancing3D.shader";
		glm::vec3 m_Ambient = glm::vec3(1.0f);
		glm::vec3 m_Diffuse = glm::vec3(1.0f);
		glm::vec3 m_Specular = glm::vec3(1.0f);
		Texture* m_BaseColor = TextureManager::GetInstance().White();
		Texture* m_NormalMap = TextureManager::GetInstance().White();
		float m_Shininess = 32.0f;
		float m_Scale = 1.0f;
		float m_Solid = 1.0f;
		bool m_UseNormalMap = false;
	};
}

