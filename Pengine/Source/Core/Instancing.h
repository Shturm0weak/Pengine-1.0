#include "Core.h"

#include "ThreadPool.h"

#include <unordered_map>
#include <condition_variable>
#include <mutex>
#include <array>
#include <vector>

namespace Pengine
{

	class PENGINE_API Instancing
	{
	private:

		const unsigned int maxTextureSlots = 32;

		struct DynamicBuffer
		{
			std::array<uint32_t, 32> m_TextureSlots;
			unsigned int m_TextureSlotsIndex = 0;
			VertexBuffer m_VboDynamic;
			VertexBufferLayout m_LayoutDynamic;
			size_t m_PrevObjectSize = 0;
			std::vector<float> m_VertexAttributes;
			bool m_AllocateNewBuffer = true;
		};

		std::unordered_map<Mesh*, DynamicBuffer> m_OpaqueBuffersByMesh;
		std::unordered_map<Mesh*, DynamicBuffer> m_ShadowsBuffersByMesh;

		// Ambient, Diffuse, Specular, transformMat4, inverseTransformMat3, BaseColorIndex, NormalMapIndex, Shininess, UseNormalMap.
		size_t m_SizeOfAttribs = 3 + 3 + 3 + 16 + 9 + 1 + 1 + 1 + 1;
		int m_TextureOffset = 5;

		friend class Renderer;
		friend class World;
		friend class Editor;
	public:
		Instancing();

		static Instancing& GetInstance();

		void Create(Mesh* mesh);

		/*
		 * Deprecated. 
		void Render(const std::unordered_map<Mesh*, std::vector<Renderer3D*>>& instancedObjects,
			std::unordered_map<Mesh*, DynamicBuffer>& bufferByMesh);
		*/

		void RenderGBuffer(const std::unordered_map<Mesh*, std::vector<Renderer3D*>>& instancedObjects,
			std::unordered_map<Mesh*, DynamicBuffer>& bufferByMesh);

		void RenderShadowsObjects(const std::unordered_map<Mesh*, std::vector<Renderer3D*>>& instancedObjects,
			std::unordered_map<Mesh*, DynamicBuffer>& bufferByMesh);

		void PrepareVertexAtrrib(const std::unordered_map<Mesh*, std::vector<Renderer3D*>>& instancedObjects);

		void BindBuffers(const std::unordered_map<Mesh*, std::vector<Renderer3D*>>& instancedObjects);

		void BindShadowsBuffers(const std::unordered_map<Mesh*, std::vector<Renderer3D*>>& instancedObjects);

		void ShutDown();
	};

}