#pragma once

#include "Core.h"
#include "Asset.h"
#include "../OpenGL/IndexBuffer.h"
#include "../OpenGL/VertexArray.h"
#include "../OpenGL/VertexBuffer.h"
#include "../OpenGL/VertexBufferLayout.h"

namespace Pengine
{

	class PENGINE_API Mesh : public IAsset
	{
	public:

		std::vector<float> m_VertexAttributes;
		std::vector<uint32_t> m_Indices;
	public:
		
		VertexArray m_Va;
		VertexBuffer m_Vb;
		IndexBuffer m_Ib;
		VertexBufferLayout m_Layout;
		std::vector<uint32_t> m_Layouts;

		Mesh(const std::string& name, std::vector<float>& vertexAttributes, std::vector<uint32_t>& indices,
			const std::vector<uint32_t>& layouts, const std::string& filePath = "None");
		
		const std::vector<float>& GetVertexAttributes() const { return m_VertexAttributes; }
		
		const std::vector<uint32_t>& GetIndices() const { return m_Indices; }
		
		void Bind() const;
		
		void UnBind() const;
		
		void Refresh();

		virtual void Reload() override;
	};

}
