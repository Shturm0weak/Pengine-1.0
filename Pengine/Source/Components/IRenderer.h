#pragma once

#include "../Core/Core.h"
#include "../Core/Component.h"
#include "../OpenGL/Shader.h"

namespace Pengine
{

	class PENGINE_API IRenderer : public IComponent
	{
	protected:

		Shader* m_Shader = Shader::Get("Default2D");
	public:

		virtual void Render() = 0;
		virtual ~IRenderer() {};
	};

}
