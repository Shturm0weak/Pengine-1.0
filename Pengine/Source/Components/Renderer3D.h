#pragma once

#include "../Core/Core.h"
#include "../Core/MeshManager.h"
#include "../Core/Utils.h"
#include "../Core/Allocator.h"
#include "../Core/TextureManager.h"
#include "IRenderer.h"
#include "../Core/MaterialManager.h"

#include <map>
#include <vector>

namespace Pengine
{

	class PENGINE_API Renderer3D : public IRenderer, public IAllocator
	{
	private:

		void EraseFromInstancing();
		void AddToInstancing();

		void EraseFromTransparent();
		void AddToTransparent();

		void EraseFromShadows();
		void AddToShadows();

		std::vector<Mesh*> m_Lods = { nullptr, nullptr, nullptr };
		std::vector<float> m_LodsDistance = { 50.0f, 100.0f };
		size_t m_CurrentLOD = 0;
		Mesh* m_Mesh = nullptr;

		friend class Serializer;
		friend class Editor;
		friend class Scene;
		friend class Raycast3D;
		friend class Renderer;
	public:

		Material* m_Material = nullptr;

		bool m_BackFaceCulling = true;
		bool m_IsOpaque = true;
		bool m_DrawShadows = true;

		Renderer3D() = default;
		
		static IComponent* Create(GameObject* owner);

		virtual IComponent* New(GameObject* owner) override;
		
		virtual void Copy(const IComponent& component) override;
		
		virtual void Render() override;
		
		virtual void Delete() override;

		void SetMesh(Mesh* mesh, size_t lod = 0);

		void SetOpaque(bool isOpaque = true);

		void SetMaterial(Material* material);

		void SetDrawShadows(bool drawShadows);

		void SetCurrentLod(size_t lod);
	};

}
