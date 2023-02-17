#pragma once

#include "../Core/Core.h"
#include "../Core/TextureManager.h"
#include "../Core/ReflectionSystem.h"
#include "../Core/Asset.h"
#include "../OpenGL/Shader.h"
#include "../Core/MaterialManager.h"

#include <array>
#include <unordered_map>

namespace Pengine
{

	class PENGINE_API Material : public IAsset
	{
		REGISTER_PARENT_CLASS(IAsset)
	public: PROPERTY(Texture*, m_BaseColor, TextureManager::GetInstance().White())
	public: PROPERTY(Texture*, m_NormalMap, TextureManager::GetInstance().White())
	public: PROPERTY(BaseMaterial*, m_BaseMaterial, MaterialManager::GetInstance().LoadBase("Source/Materials/Material.basemat"))
	public: PROPERTY(glm::vec3, m_Ambient, glm::vec3(1.0f))
	public: PROPERTY(float, m_Scale, 1.0f)
	public: PROPERTY(float, m_Solid, 1.0f)
	public: PROPERTY(float, m_Shiness, 32.0f)
	public: PROPERTY(bool, m_UseNormalMap, false)
	public: int m_UniformIndex;
	private: uint32_t m_BaseColorIndex = 0;
	private: uint32_t m_NormalMapIndex = 0;
		friend class Renderer;
		friend class Renderer3D;
		friend class Instancing;
	};
	REGISTER_CLASS(Material)
}

