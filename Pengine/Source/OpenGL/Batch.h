#pragma once

#include "IndexBuffer.h"

#include <array>
#include <vector>

namespace Pengine
{

#define RENDERER_MAX_SPRITES   20000
#define RENDERER_INDICES_SIZE  RENDERER_MAX_SPRITES * 6

#define RENDERER_MAX_LINES   20000
#define RENDERER_INDICES_SIZE_LINES  RENDERER_MAX_LINES * 2

	class PENGINE_API Batch 
	{
	private:

		struct UIVertex {
			glm::vec2 m_Vertex;
			glm::vec2 m_UV;
			int m_Static;
			glm::vec4 m_Color;
			float m_IsGui;
			float m_TexIndex;
			int m_IsRelatedToPanel;
			glm::vec2 m_RelatedPanelPosition;
			glm::vec2 m_RelatedPanelSize;
			glm::vec4 m_RoundedCorners;
			glm::vec4 m_PreviousRoundedCorners;
			glm::vec2 m_Position;
			glm::vec2 m_Size;
		};

		struct LineVertex
		{
			glm::vec3 m_Position;
			glm::vec4 m_Color;
		};

		struct GOVertex
		{
			glm::vec3 m_Position;
			glm::vec2 m_UV;
			glm::vec4 m_Color;
			glm::vec4 m_TransformMat0;
			glm::vec4 m_TransformMat1;
			glm::vec4 m_TransformMat2;
			glm::vec4 m_TransformMat3;
			float m_TextureIndex;
		};

		struct GOVertAttribWrapper
		{
			std::array<uint32_t, 32> m_TextureSlots;
			const uint32_t m_StartTextureSlotIndex = 1;
			uint32_t m_TextureSlotIndex = m_StartTextureSlotIndex;
			float m_TextureIndex = 0;

			GOVertex* m_Buffer = nullptr;
			GOVertex* m_BufferPtr = nullptr;
			GLuint m_Vao;
			GLuint m_Vbo;
			IndexBuffer m_Ibo;
			GLsizei m_IndexCount = 0;

			~GOVertAttribWrapper()
			{
				glDeleteBuffers(1, &m_Vbo);
			}
		};

		struct UIVertAttribWrapper
		{
			std::array<uint32_t, 32> m_TextureSlots;
			const uint32_t m_StartTextureSlotIndex = 1;
			uint32_t m_TextureSlotIndex = m_StartTextureSlotIndex;
			float m_TextureIndex = 0;

			UIVertex* m_Buffer = nullptr;
			UIVertex* m_BufferPtr = nullptr;
			GLuint m_Vao;
			GLuint m_Vbo;
			IndexBuffer m_Ibo;
			GLsizei m_IndexCount = 0;

			~UIVertAttribWrapper()
			{
				glDeleteBuffers(1, &m_Vbo);
			}
		};

		struct LineVertAttribWrapper
		{
			LineVertex* m_Buffer = nullptr;
			LineVertex* m_BufferPtr = nullptr;
			GLuint m_Vao;
			GLuint m_Vbo;
			IndexBuffer m_Ibo;
			GLsizei m_IndexCount = 0;

			~LineVertAttribWrapper()
			{
				glDeleteBuffers(1, &m_Vbo);
			}
		};

		UIVertAttribWrapper m_UIWrapper;
		GOVertAttribWrapper m_GOWrapper;
		LineVertAttribWrapper m_LineWrapper;
		
		const uint32_t m_MaxTextureSlots = 32;
		
		void InitializeGameObjects();
		void InitializeLines();
		void InitializeUI();

		Batch();
		Batch(const Batch&) = delete;
		Batch& operator=(const Batch&) { return *this; }
		~Batch();
	public:

		static Batch& GetInstance();

		void Submit(class Character* character);
		void Submit(float* mesh, const glm::vec4& color, const glm::vec2& position, const glm::vec2& size, class Texture* texture = nullptr);
		void Submit(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color);
		void Submit(const std::vector<float>& vertices, const std::vector<float>& uv, const glm::mat4& transform,
			const glm::vec4& color, class Texture* texture = nullptr);
		void Submit(const std::vector<float>& mesh, const glm::mat4& transform,
			const glm::vec4& color, class Texture* texture = nullptr);
		void Submit(const std::vector<float>& mesh, const glm::mat4& positionMat4, const glm::mat4& rotationMat4,
			const glm::mat4& scaleMat4, const glm::vec4& color, class Texture* texture = nullptr);
		
		void FlushGameObjects();
		void FlushLines();
		void FlushUI();

		void BeginLines();
		void EndLines();
		void BeginGameObjects();
		void EndGameObjects();
		void BeginUI();
		void EndUI();
	};

}