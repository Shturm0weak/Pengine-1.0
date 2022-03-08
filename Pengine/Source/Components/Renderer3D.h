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

		Mesh* m_Mesh = nullptr;
		Material m_Material;

		friend class Serializer;
	public:

		static IComponent* Create(GameObject* owner);

		virtual void Copy(const IComponent& component) override;
		virtual IComponent* Renderer3D::CreateCopy(GameObject* newOwner) override;
		virtual void Render() override;
		virtual void Delete() override;
		Renderer3D() = default;

		void SetMesh(Mesh* mesh) { m_Mesh = mesh; }

		void operator=(const Renderer3D& Renderer3D);
	};

}
