#include "Core.h"
#include "Scene.h"
#include "../OpenGL/BaseMaterial.h"

#include <unordered_map>

namespace Pengine
{

	class PENGINE_API Instancing
	{
	private:
		// transformMat4, inverseTransformMat3, materialIndex.
		const size_t m_SizeOfAttribs = 16 + 9 + 1;

		friend class Renderer;
		friend class World;
		friend class Editor;
	public:
		Instancing();

		static Instancing& GetInstance();

		/*
		 * Deprecated. 
		void Render(const std::unordered_map<Mesh*, std::vector<Renderer3D*>>& instancedObjects,
			std::unordered_map<Mesh*, DynamicBuffer>& bufferByMesh);
		*/

		void RenderGBuffer(const std::unordered_map<BaseMaterial*, Scene::Renderable>& instancedObjects);

		void RenderShadowsObjects(const std::unordered_map<Mesh*, std::vector<Renderer3D*>>& instancedObjects);

		void PrepareVertexAtrrib(const std::unordered_map<BaseMaterial*, Scene::Renderable>& instancedObjects);

		void PrepareShadowsVertexAtrrib(const std::unordered_map<Mesh*, std::vector<Renderer3D*>>& instancedObjects);

		void BindShadowsBuffers(const std::unordered_map<Mesh*, std::vector<Renderer3D*>>& instancedObjects);
	};

}