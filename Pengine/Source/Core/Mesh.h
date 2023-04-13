#pragma once

#include "Core.h"
#include "Asset.h"
#include "BoundingBox.h"
#include "../OpenGL/IndexBuffer.h"
#include "../OpenGL/VertexArray.h"
#include "../OpenGL/VertexBuffer.h"
#include "../OpenGL/VertexBufferLayout.h"

namespace Pengine
{

	class PENGINE_API Mesh : public IAsset
	{
	public:
		struct Meta
		{
			std::string m_Name;
			std::string m_FilePath;
			int m_ShadowCullFace = GL_BACK;
			bool m_CullFace = true;
		};

		struct DynamicBuffer
		{
			VertexBuffer m_VboDynamic;
			VertexBufferLayout m_LayoutDynamic;
			size_t m_PrevObjectSize = 0;
			std::vector<float> m_VertexAttributes;
			bool m_AllocateNewBuffer = true;
			bool m_Initialized = false;

			void Initialize();
		};

		struct DynamicBufferShadows
		{
			VertexBuffer m_VboDynamic;
			VertexBufferLayout m_LayoutDynamic;
			size_t m_PrevObjectSize = 0;
			std::vector<float> m_VertexAttributes;
			bool m_AllocateNewBuffer = true;
			bool m_Initialized = false;

			void Initialize();
		};

		struct BoneInfo
		{
			glm::mat4 offset;
			int id;
		};

		std::unordered_map<std::string, BoneInfo> m_BoneInfoByName;
		std::vector<float> m_VertexAttributes;
		std::vector<uint32_t> m_Indices;

		std::unordered_map<class BaseMaterial*, DynamicBuffer> m_DynamicBufferOpaqueByMaterial;
		DynamicBufferShadows m_DynamicBufferShadows;

		BoundingBox m_BoundingBox;

		VertexArray m_Va;
		VertexBuffer m_Vb;
		IndexBuffer m_Ib;
		VertexBufferLayout m_Layout;
		std::vector<uint32_t> m_Layouts;
		int m_ShadowCullFace = GL_FRONT_AND_BACK;
		bool m_CullFace = true;

		Mesh(const std::string& name, std::vector<float>& vertexAttributes, std::vector<uint32_t>& indices,
			const std::vector<uint32_t>& layouts, const std::string& filePath);
		
		static size_t GetVertexOffset() { return 22; }

		static size_t GetStaticAttributeVertexOffset() { return 7; }

		const std::vector<float>& GetVertexAttributes() const { return m_VertexAttributes; }
		
		const std::vector<uint32_t>& GetIndices() const { return m_Indices; }

		void Bind() const;
		
		void UnBind() const;
		
		void Refresh();
	};

}
