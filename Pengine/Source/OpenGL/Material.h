#pragma once

#include "../Core/Core.h"
#include "../Core/TextureManager.h"
#include "../Core/ReflectionSystem.h"
#include "../Core/Asset.h"

namespace Pengine
{

	class PENGINE_API Material : public IAsset
	{
		REGISTER_PARENT_CLASS(IAsset)
	public: PROPERTY(Texture*, m_BaseColor, TextureManager::GetInstance().White())
	public: PROPERTY(Texture*, m_NormalMap, TextureManager::GetInstance().White())
	public: PROPERTY(std::string, m_ShaderFilePath, "Instancing3D.shader")
	public: PROPERTY(glm::vec3, m_Ambient, glm::vec3(1.0f))
	public: PROPERTY(glm::vec3, m_Diffuse, glm::vec3(1.0f))
	public: PROPERTY(glm::vec3, m_Specular, glm::vec3(1.0f))
	public: PROPERTY(float, m_Shininess, 32.0f)
	public: PROPERTY(float, m_Scale, 1.0f)
	public: PROPERTY(float, m_Solid, 1.0f)
	public: PROPERTY(bool, m_UseNormalMap, false)
	};
	REGISTER_CLASS(Material)
}

