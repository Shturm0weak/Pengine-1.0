#pragma once

#include "../Core/Core.h"
#include "../Core/TextureManager.h"
#include "../Core/ReflectionSystem.h"
#include "../Core/Asset.h"
#include "../OpenGL/Shader.h"

#include <array>
#include <vector>

namespace Pengine
{

	class PENGINE_API BaseMaterial : public IAsset
	{
		REGISTER_PARENT_CLASS(IAsset)
	public: PROPERTY(Shader*, m_Shader, Shader::Get("InstancingGBuffer"))
	public: std::vector<class Material*> m_Inherited;	
	public: std::unordered_map<std::string, float> m_FloatUniformsByName;
	public: std::unordered_map<std::string, int> m_IntUniformsByName;
	public: std::unordered_map<std::string, glm::vec2> m_Vec2UniformsByName;
	public: std::unordered_map<std::string, glm::vec3> m_Vec3UniformsByName;
	public: std::unordered_map<std::string, glm::vec4> m_Vec4UniformsByName;
	public: std::unordered_map<std::string, Texture*> m_TextureUniformsByName;
	private:
		
		std::array<uint32_t, MAX_TEXTURE_SLOTS> m_TextureSlots;
		std::array<GLenum, MAX_TEXTURE_SLOTS> m_TextureSlotsTarget;
		uint32_t m_TextureSlotsIndex = 0;
	public:

		int BindTexture(uint32_t id, GLenum target = GL_TEXTURE_2D);

		std::vector<int> BindTextures();

		void UnBindTextures();
	};
	REGISTER_CLASS(BaseMaterial)
}

