#pragma once

#include "Core.h"
#include "../OpenGL/IndexBuffer.h"
#include "../OpenGL/VertexArray.h"
#include "../OpenGL/VertexBuffer.h"
#include "../OpenGL/VertexBufferLayout.h"

namespace Pengine
{

	class PENGINE_API Mesh
	{
	private:

		std::vector<float> m_VertexAttributes;
		std::vector<uint32_t> m_Indices;

		std::string m_Name;
		std::string m_FilePath = "None";
	public:
		
		VertexArray m_Va;
		VertexBuffer m_Vb;
		VertexBufferLayout m_Layout;
		IndexBuffer m_Ib;

		Mesh(const std::string& name, std::vector<float>& vertexAttributes, std::vector<uint32_t>& indices,
			const std::vector<uint32_t>& layouts, const std::string& filePath = "None");

		std::string GetFilePath() const { return m_FilePath; }
		std::string GetName() const { return m_Name; }
		const std::vector<float>& GetVertexAttributes() const { return m_VertexAttributes; }
		const std::vector<uint32_t>& GetIndices() const { return m_Indices; }
		void Bind() const;
		void UnBind() const;
	};

}
