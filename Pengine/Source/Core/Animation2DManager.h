#pragma once

#include "Core.h"
#include "Mesh.h"
#include "TextureManager.h"
#include "Utils.h"

#include <vector>
#include <unordered_map>

namespace Pengine
{

	class PENGINE_API Animation2DManager
	{
	public:

		class Animation2D : public IAsset
		{
		private:

			std::vector<Texture*> m_Textures;
			std::vector<std::vector<float>> m_UVs;

			friend class Animation2DManager;
			friend class Animator2D;
			friend class Editor;
			friend class Serializer;
		public:

			std::vector<Texture*> GetTextures() const { return m_Textures; }
			
			std::vector<std::vector<float>> GetUVs() const { return m_UVs; }

			Animation2D(const std::string& filePath, const std::string& name)
			{
				SetFilePath(filePath);
				SetName(name);
			}

			virtual void Reload() override {}

			void operator=(const Animation2D& animation)
			{
				m_Textures = animation.m_Textures;
				m_FilePath = animation.m_FilePath;
				m_Name = animation.m_Name;
				m_UVs = animation.m_UVs;
			}
		};
	private:

		std::unordered_map<std::string, Animation2D*> m_Animations;

		Animation2DManager() = default;
		Animation2DManager(const Animation2DManager&) = delete;
		Animation2DManager& operator=(const Animation2DManager&) { return *this; }
		~Animation2DManager() = default;

		friend class Editor;
	public:

		static Animation2DManager& GetInstance();

		void Save(Animation2D* animation);
		
		Animation2D* Load(const std::string& filePath);
		
		Animation2D* Create(const std::string& filePath);
		
		Animation2D* Get(const std::string& filePath) const;

		void ShutDown();
	};

}
