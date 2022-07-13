#pragma once

#include "../Core/Core.h"
#include "../Core/MeshManager.h"
#include "../Core/Utils.h"
#include "../Core/Allocator.h"
#include "../Core/TextureManager.h"
#include "IRenderer.h"
#include "../OpenGL/Material.h"

#include <map>

namespace Pengine
{

	class PENGINE_API Renderer3D : public IRenderer, public IAllocator
	{
	private:

		Material m_Material;

		friend class Serializer;
	public:

		Mesh* m_Mesh = nullptr;

		Renderer3D() = default;
		
		static IComponent* Create(GameObject* owner);

		virtual IComponent* New(GameObject* owner) override;
		
		virtual void Copy(const IComponent& component) override;
		
		virtual void Render() override;
		
		virtual void Delete() override;

		void SetMesh(Mesh* mesh) { m_Mesh = mesh; }
	};

}
