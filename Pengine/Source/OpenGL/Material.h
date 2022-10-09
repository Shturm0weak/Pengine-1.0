#pragma once

#include "../Core/Core.h"
#include "../Core/TextureManager.h"
#include "../Core/ReflectionSystem.h"
#include "../Core/Asset.h"
//#include "Shader.h"

namespace Pengine
{

	class PENGINE_API Material : public IAsset
	{
		RTTR_ENABLE(IAsset)

		private: PROPERTY(Material, std::string, m_BaseColorFilePath, "White")
		private: PROPERTY(Material, std::string, m_ShaderFilePath, "Instancing3D.shader")
		public: PROPERTY(Material, glm::vec3, m_Ambient, glm::vec3(1.0f))
		public: PROPERTY(Material, glm::vec3, m_Diffuse, glm::vec3(1.0f))
		public: PROPERTY(Material, glm::vec3, m_Specular, glm::vec3(1.0f))
		public: PROPERTY(Material, float, m_Shininess, 32.0f)
		public: PROPERTY(Material, float, m_Scale, 1.0f)
		public: PROPERTY(Material, float, m_Solid, 1.0f)
	private:
		
		bool m_IsInherited = false;

		friend class Serializer;
	public:

		bool IsInherited() const { return (char)m_IsInherited == 1; }

		Texture* m_BaseColor = TextureManager::GetInstance().White();

		virtual void Reload() override {}

		Material* Inherit();

		void SetBaseColor(class Texture* texture, const std::string& filePath);
	};
	REGISTER_ASSET(Material)
	
	/*class PENGINE_API Material
	{
	public:
		glm::vec3 m_Ambient = { 1.0f, 1.0f, 1.0f };
		glm::vec3 m_Diffuse = { 1.0f, 1.0f, 1.0f };
		glm::vec3 m_Specular = { 1.0f, 1.0f, 1.0f };
		float m_Shininess = 32.0f;
		float m_Scale = 1.0f;
		float m_Solid = 1.0f;
		Texture* m_BaseColor = TextureManager::GetInstance().White();

		void operator=(const Material& material);
	};*/
}

