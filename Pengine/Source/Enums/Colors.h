#pragma once

#include "../Core/Core.h"

namespace Pengine
{

	class PENGINE_API Colors
	{
	public:
		static glm::vec4 Red() { return { 1.0f, 0.0f, 0.0f, 1.0f }; }
		static glm::vec4 Green() { return { 0.0f, 1.0f, 0.0f, 1.0f }; }
		static glm::vec4 White() { return { 1.0f, 1.0f, 1.0f, 1.0f }; }
		static glm::vec4 Blue() { return { 0.0f, 0.0f, 1.0f, 1.0f }; }
		static glm::vec4 Yellow() { return { 1.0f, 1.0f, 0.0f, 1.0f }; }
	};

}