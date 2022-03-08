#pragma once

#include "Core.h"
#include "Mesh.h"
#include "TextureManager.h"
#include "Utils.h"

#include <unordered_map>

namespace Pengine
{

	class PENGINE_API Animation2DManager
	{
	public:

		struct Animation2D
		{
			std::vector<Texture*> m_Textures;
			std::string m_FilePath;
			std::string m_Name;

			Animation2D(const std::string& filePath, const std::string& name)
				: m_FilePath(filePath)
				, m_Name(name)
			{
			}

			void operator=(const Animation2D& animation)
			{
				m_Textures = animation.m_Textures;
				m_FilePath = animation.m_FilePath;
				m_Name = animation.m_Name;
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

		bool m_ReplaceCurrentAnimFile = true;

		static Animation2DManager& GetInstance();

		void Save(Animation2D* animation);
		Animation2D* Load(const std::string& filePath);
		Animation2D* Create(const std::string& name);
		Animation2D* Get(const std::string& name) const;

		void ShutDown();
	};

}
