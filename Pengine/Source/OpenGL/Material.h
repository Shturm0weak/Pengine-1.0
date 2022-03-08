#pragma once

#include "../Core/Core.h"
#include "../Core/TextureManager.h"

namespace Pengine
{
	struct PENGINE_API Material
	{
		glm::vec4 m_Color = { 1.0f, 1.0f, 1.0f, 1.0f };
		Texture* m_Texture = TextureManager::GetInstance().Get("White");
	};
	
}