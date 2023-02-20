#pragma once

#include "../Core/Core.h"
#include "../Core/Component.h"
#include "../OpenGL/Shader.h"

namespace Pengine
{

	class PENGINE_API IRenderer : public IComponent
	{
	protected:

		virtual void Render() = 0;
	public:

		virtual ~IRenderer() {};
	};

}
