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
	public: PROPERTY(Texture*, m_MetallicMap, TextureManager::GetInstance().White())
	public: PROPERTY(Texture*, m_RoughnessMap, TextureManager::GetInstance().White())
	public: PROPERTY(Texture*, m_AoMap, TextureManager::GetInstance().White())
	public: PROPERTY(BaseMaterial*, m_BaseMaterial, MaterialManager::GetInstance().LoadBase("Source/Materials/Material.basemat"))
	public: PROPERTY(glm::vec3, m_Albedo, glm::vec3(1.0f))
	public: PROPERTY(glm::vec4, m_UvTransform, glm::vec4(1.0f, 1.0f, 0.0f, 0.0f))
	public: PROPERTY(float, m_Intensity, 1.0f)
	public: PROPERTY(float, m_Metallic, 0.05f)
	public: PROPERTY(float, m_Roughness, 1.0f)
	public: PROPERTY(bool, m_UseMetallicMap, false)
	public: PROPERTY(bool, m_UseRoughnessMap, false)
	public: PROPERTY(bool, m_UseAoMap, false)
	public: PROPERTY(float, m_Alpha, 1.0f)
	public: PROPERTY(bool, m_UseNormalMap, false)
	public: int m_UniformIndex;
	private: uint32_t m_BaseColorIndex = 0;
	private: uint32_t m_NormalMapIndex = 0;
	private: uint32_t m_MetallicIndex = 0;
	private: uint32_t m_RoughnessIndex = 0;
	private: uint32_t m_AoIndex = 0;
		friend class Renderer;
		friend class Renderer3D;
		friend class Instancing;
	};
	REGISTER_CLASS(Material)
}

