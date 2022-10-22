#include "Core.h"

#include "ThreadPool.h"

#include <unordered_map>
#include <condition_variable>
#include <mutex>
#include <array>

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
			size_t m_Size = 0;
			float* m_VertAttrib = nullptr;
			bool m_AllocateNewBuffer = true;
		};

		std::unordered_map<Mesh*, DynamicBuffer> m_OpaqueBuffersByMesh;
		std::unordered_map<Mesh*, DynamicBuffer> m_ShadowsBuffersByMesh;
		size_t m_NThreads;
		ThreadPool::SyncParams m_SyncParams;

		// Ambient, Diffuse, Specular, transformMat4, TextureIndex, Shininess
		size_t m_SizeOfAttribs = 3 + 3 + 3 + 16 + 1 + 1;
		int m_SizeOfObjectToThreads = 100;
		int m_TextureOffset = 5;

		friend class Renderer;
		friend class World;
		friend class Editor;
	public:
		Instancing();

		static Instancing& GetInstance();

		void Create(Mesh* mesh);

		void Render(const std::unordered_map<Mesh*, std::vector<Renderer3D*>>& instancedObjects,
			std::unordered_map<Mesh*, DynamicBuffer>& bufferByMesh);

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