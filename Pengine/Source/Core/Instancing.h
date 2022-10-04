#include "Core.h"

#include "ThreadPool.h"

#include <map>
#include <condition_variable>
#include <mutex>
#include <array>

namespace Pengine
{

	class PENGINE_API Instancing
	{
	private:

		const unsigned int maxTextureSlots = 32;

		struct glBuffers {
			std::array<uint32_t, 32> m_TextureSlots;
			unsigned int m_TextureSlotsIndex = 0;
			VertexBuffer m_VboDynamic;
			VertexBufferLayout m_LayoutDynamic;
			size_t m_PrevObjectSize = 0;
			float* m_VertAttrib = nullptr;
		};

		std::map<Mesh*, glBuffers> m_Buffers;
		size_t m_NThreads;
		ThreadPool::SyncParams m_SyncParams;

		// Position, Scale, Ambient, Diffuse, Specular, RotationMat4, TextureIndex, Shininess
		size_t m_SizeOfAttribs = 3 + 3 + 3 + 3 + 3 + 16 + 1 + 1;
		int m_SizeOfObjectToThreads = 100;

		friend class Renderer;
		friend class World;
		friend class Editor;
	public:

		bool m_FinishPrepareVertAtribb = true;

		Instancing();

		static Instancing& GetInstance();
		void Create(Mesh* mesh);
		void Render(const std::map<Mesh*, std::vector<Renderer3D*>>& instancedObjects);
		void PrepareVertexAtrrib(const std::map<Mesh*, std::vector<Renderer3D*>>& instancedObjects);
		void ShutDown();
	};

}