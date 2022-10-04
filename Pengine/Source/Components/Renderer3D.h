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

		void EraseFromInstancing();
		void AddToInstancing();

		Mesh* m_Mesh = nullptr;

		friend class Serializer;
		friend class Editor;
		friend class Scene;
		friend class Raycast3D;
	public:

		Material m_Material;

		bool m_BackFaceCulling = true;

		Renderer3D() = default;
		
		static IComponent* Create(GameObject* owner);

		virtual IComponent* New(GameObject* owner) override;
		
		virtual void Copy(const IComponent& component) override;
		
		virtual void Render() override;
		
		virtual void Delete() override;

		void SetMesh(Mesh* mesh);
	};

}
