#include "Batch.h"

#include "Shader.h"
#include "../Core/TextureManager.h"
#include "../Core/Window.h"
#include "../Core/Environment.h"
#include "../Core/Logger.h"
#include "../Core/Editor.h"
#include "../Core/Viewport.h"
#include "../Core/EntryPoint.h"
#include "../UI/Gui.h"

using namespace Pengine;

void Batch::InitializeGameObjects()
{
	glGenVertexArrays(1, &m_GOWrapper.m_Vao);
	glBindVertexArray(m_GOWrapper.m_Vao);

	glGenBuffers(1, &m_GOWrapper.m_Vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_GOWrapper.m_Vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GOVertex) * 4 * RENDERER_MAX_SPRITES, NULL, GL_DYNAMIC_DRAW);

	glBindVertexArray(m_GOWrapper.m_Vao);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GOVertex), (const GLvoid*)offsetof(GOVertex, m_Position));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GOVertex), (const GLvoid*)offsetof(GOVertex, m_UV));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(GOVertex), (const GLvoid*)offsetof(GOVertex, m_Color));

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(GOVertex), (const GLvoid*)offsetof(GOVertex, m_TransformMat0));

	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(GOVertex), (const GLvoid*)offsetof(GOVertex, m_TransformMat1));

	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(GOVertex), (const GLvoid*)offsetof(GOVertex, m_TransformMat2));

	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(GOVertex), (const GLvoid*)offsetof(GOVertex, m_TransformMat3));

	glEnableVertexAttribArray(7);
	glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(GOVertex), (const GLvoid*)offsetof(GOVertex, m_AdditionalData0));

	glEnableVertexAttribArray(8);
	glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(GOVertex), (const GLvoid*)offsetof(GOVertex, m_AdditionalData1));

	glEnableVertexAttribArray(9);
	glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, sizeof(GOVertex), (const GLvoid*)offsetof(GOVertex, m_TextureData));

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	unsigned int indices[RENDERER_INDICES_SIZE];

	unsigned int offset = 0;
	for (unsigned int i = 0; i < RENDERER_INDICES_SIZE; i += 6)
	{
		indices[i + 0] = offset + 0;
		indices[i + 1] = offset + 1;
		indices[i + 2] = offset + 2;
		indices[i + 3] = offset + 2;
		indices[i + 4] = offset + 3;
		indices[i + 5] = offset + 0;

		offset += 4;
	}

	m_GOWrapper.m_BufferPtr = new GOVertex[RENDERER_MAX_SPRITES * 4];
	m_GOWrapper.m_Ibo.Initialize(indices, RENDERER_INDICES_SIZE);
	glBindVertexArray(0);

	m_GOWrapper.m_TextureSlots[0] = TextureManager::GetInstance().White()->GetRendererID();
	for (unsigned int i = m_GOWrapper.m_StartTextureSlotIndex; i < 32; i++)
	{
		m_GOWrapper.m_TextureSlots[i] = 0;
	}
}

void Batch::InitializeLines()
{
	glGenVertexArrays(1, &m_LineWrapper.m_Vao);
	glBindVertexArray(m_LineWrapper.m_Vao);

	glGenBuffers(1, &m_LineWrapper.m_Vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_LineWrapper.m_Vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(LineVertex) * 2 * RENDERER_MAX_LINES, NULL, GL_DYNAMIC_DRAW);

	glBindVertexArray(m_LineWrapper.m_Vao);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(LineVertex), (const GLvoid*)offsetof(LineVertex, m_Position));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(LineVertex), (const GLvoid*)offsetof(LineVertex, m_Color));

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	unsigned int indices[RENDERER_INDICES_SIZE_LINES];

	unsigned int offset = 0;
	for (unsigned int i = 0; i < RENDERER_INDICES_SIZE_LINES; i += 2)
	{
		indices[i + 0] = offset + 0;
		indices[i + 1] = offset + 1;

		offset += 2;
	}

	m_LineWrapper.m_Ibo.Initialize(indices, RENDERER_INDICES_SIZE_LINES);
	m_LineWrapper.m_BufferPtr = new LineVertex[RENDERER_MAX_LINES];
	glBindVertexArray(0);
}

void Batch::InitializeUI()
{
	glGenVertexArrays(1, &m_UIWrapper.m_Vao);
	glBindVertexArray(m_UIWrapper.m_Vao);

	glGenBuffers(1, &m_UIWrapper.m_Vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_UIWrapper.m_Vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(UIVertex) * 4 * RENDERER_MAX_SPRITES, NULL, GL_DYNAMIC_DRAW);

	glBindVertexArray(m_UIWrapper.m_Vao);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(UIVertex), (const GLvoid*)offsetof(UIVertex, m_Vertex));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(UIVertex), (const GLvoid*)offsetof(UIVertex, m_UV));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 1, GL_INT, GL_FALSE, sizeof(UIVertex), (const GLvoid*)offsetof(UIVertex, m_Static));

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(UIVertex), (const GLvoid*)offsetof(UIVertex, m_Color));

	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(UIVertex), (const GLvoid*)offsetof(UIVertex, m_IsGui));

	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, sizeof(UIVertex), (const GLvoid*)offsetof(UIVertex, m_TexIndex));

	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 1, GL_INT, GL_FALSE, sizeof(UIVertex), (const GLvoid*)offsetof(UIVertex, m_IsRelatedToPanel));

	glEnableVertexAttribArray(7);
	glVertexAttribPointer(7, 2, GL_FLOAT, GL_FALSE, sizeof(UIVertex), (const GLvoid*)offsetof(UIVertex, m_RelatedPanelPosition));

	glEnableVertexAttribArray(8);
	glVertexAttribPointer(8, 2, GL_FLOAT, GL_FALSE, sizeof(UIVertex), (const GLvoid*)offsetof(UIVertex, m_RelatedPanelSize));

	glEnableVertexAttribArray(9);
	glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, sizeof(UIVertex), (const GLvoid*)offsetof(UIVertex, m_RoundedCorners));

	glEnableVertexAttribArray(10);
	glVertexAttribPointer(10, 4, GL_FLOAT, GL_FALSE, sizeof(UIVertex), (const GLvoid*)offsetof(UIVertex, m_PreviousRoundedCorners));

	glEnableVertexAttribArray(11);
	glVertexAttribPointer(11, 2, GL_FLOAT, GL_FALSE, sizeof(UIVertex), (const GLvoid*)offsetof(UIVertex, m_Position));

	glEnableVertexAttribArray(12);
	glVertexAttribPointer(12, 2, GL_FLOAT, GL_FALSE, sizeof(UIVertex), (const GLvoid*)offsetof(UIVertex, m_Size));

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	unsigned int indices[RENDERER_INDICES_SIZE];

	unsigned int offset = 0;
	for (unsigned int i = 0; i < RENDERER_INDICES_SIZE; i += 6)
	{
		indices[i + 0] = offset + 0;
		indices[i + 1] = offset + 1;
		indices[i + 2] = offset + 2;
		indices[i + 3] = offset + 2;
		indices[i + 4] = offset + 3;
		indices[i + 5] = offset + 0;

		offset += 4;
	}

	m_UIWrapper.m_BufferPtr = new UIVertex[RENDERER_MAX_SPRITES];
	m_UIWrapper.m_Ibo.Initialize(indices, RENDERER_INDICES_SIZE);
	glBindVertexArray(0);

	m_UIWrapper.m_TextureSlots[0] = TextureManager::GetInstance().White()->GetRendererID();
	for (unsigned int i = 1; i < 32; i++)
	{
		m_UIWrapper.m_TextureSlots[i] = 0;
	}
}

float Batch::GOVertAttribWrapper::GetTextureSlot(uint32_t maxTextureSlots, Texture* texture)
{
	float textureIndex = -1.0f;

	if (texture == nullptr)
	{
		texture = TextureManager::GetInstance().White();
	}

	for (unsigned int i = 0; i < maxTextureSlots; i++)
	{
		if (m_TextureSlots[i] == texture->GetRendererID())
		{
			textureIndex = (float)i;
			break;
		}
	}

	if (textureIndex == -1.0f)
	{
		textureIndex = (float)m_TextureSlotIndex;
		m_TextureSlots[m_TextureSlotIndex] = texture->GetRendererID();
		m_TextureSlotIndex++;
	}

	return textureIndex;
}

Batch::Batch()
{
	InitializeGameObjects();
	InitializeLines();
	InitializeUI();
	Logger::Success("Batch renderer has been initialized!");
}

Batch::~Batch()
{
	delete[] m_GOWrapper.m_BufferPtr;
	delete[] m_LineWrapper.m_BufferPtr;
}

Batch& Batch::GetInstance()
{
	static Batch batch;
	return batch;
}

void Batch::Submit(Character* character)
{
	if (m_UIWrapper.m_TextureSlotIndex > m_MaxTextureSlots - 1 || m_UIWrapper.m_IndexCount >= RENDERER_INDICES_SIZE)
	{
		Batch::GetInstance().EndUI();
		Batch::GetInstance().FlushUI();
		Batch::GetInstance().BeginUI();
	}

	m_UIWrapper.m_TextureIndex = -1.0f;
	if (character->m_Font->m_FontAtlas == nullptr)
	{
		character->m_Font->m_FontAtlas = TextureManager::GetInstance().White();
	}

	for (unsigned int i = 0; i < m_MaxTextureSlots; i++)
	{
		if (m_UIWrapper.m_TextureSlots[i] == character->m_Font->m_FontAtlas->GetRendererID())
		{
			m_UIWrapper.m_TextureIndex = (float)i;
			break;
		}

	}
	if (m_UIWrapper.m_TextureIndex == -1.0f)
	{
		m_UIWrapper.m_TextureIndex = (float)m_UIWrapper.m_TextureSlotIndex;
		m_UIWrapper.m_TextureSlots[m_UIWrapper.m_TextureSlotIndex] = character->m_Font->m_FontAtlas->GetRendererID();
		m_UIWrapper.m_TextureSlotIndex++;
	}

	Gui& gui = Gui::GetInstance();

	bool IsStatic = gui.m_Theme.m_IsStatic;

	float aspect = Viewport::GetInstance().GetSize().y / gui.m_RelativeHeight;
	
	glm::vec2 viewportSize = (glm::vec2)Viewport::GetInstance().GetSize();
	glm::vec2 relatedPanelPosition = gui.m_CurrentPanelPosition * aspect + viewportSize * 0.5f;

	glm::vec2 relatedPanelSize = gui.m_CurrentPanelSize * aspect;

	m_UIWrapper.m_Buffer->m_Vertex = glm::vec2((character->m_Mesh2D[0] * character->m_Scale.x + character->m_Position.x), ((character->m_Mesh2D[1]) * character->m_Scale.y + character->m_Position.y));
	m_UIWrapper.m_Buffer->m_UV = glm::vec2(character->m_Mesh2D[2], character->m_Mesh2D[3]);
	m_UIWrapper.m_Buffer->m_Static = IsStatic;
	m_UIWrapper.m_Buffer->m_Color = character->m_Color;
	m_UIWrapper.m_Buffer->m_IsGui = 0.0f;
	m_UIWrapper.m_Buffer->m_TexIndex = m_UIWrapper.m_TextureIndex;
	m_UIWrapper.m_Buffer->m_IsRelatedToPanel = gui.m_IsRelatedToMainPanel;
	m_UIWrapper.m_Buffer->m_RelatedPanelPosition = relatedPanelPosition;
	m_UIWrapper.m_Buffer->m_RelatedPanelSize = relatedPanelSize;
	m_UIWrapper.m_Buffer++;

	m_UIWrapper.m_Buffer->m_Vertex = glm::vec2((character->m_Mesh2D[4] * character->m_Scale.x + character->m_Position.x), ((character->m_Mesh2D[5]) * character->m_Scale.y + character->m_Position.y));
	m_UIWrapper.m_Buffer->m_UV = glm::vec2(character->m_Mesh2D[6], character->m_Mesh2D[7]);
	m_UIWrapper.m_Buffer->m_Static = IsStatic;
	m_UIWrapper.m_Buffer->m_Color = character->m_Color;
	m_UIWrapper.m_Buffer->m_IsGui = 0.0f;
	m_UIWrapper.m_Buffer->m_TexIndex = m_UIWrapper.m_TextureIndex;
	m_UIWrapper.m_Buffer->m_IsRelatedToPanel = gui.m_IsRelatedToMainPanel;
	m_UIWrapper.m_Buffer->m_RelatedPanelPosition = relatedPanelPosition;
	m_UIWrapper.m_Buffer->m_RelatedPanelSize = relatedPanelSize;
	m_UIWrapper.m_Buffer++;

	m_UIWrapper.m_Buffer->m_Vertex = glm::vec2((character->m_Mesh2D[8] * character->m_Scale.x + character->m_Position.x), ((character->m_Mesh2D[9]) * character->m_Scale.y + character->m_Position.y));
	m_UIWrapper.m_Buffer->m_UV = glm::vec2(character->m_Mesh2D[10], character->m_Mesh2D[11]);
	m_UIWrapper.m_Buffer->m_Static = IsStatic;
	m_UIWrapper.m_Buffer->m_Color = character->m_Color;
	m_UIWrapper.m_Buffer->m_IsGui = 0.0f;
	m_UIWrapper.m_Buffer->m_TexIndex = m_UIWrapper.m_TextureIndex;
	m_UIWrapper.m_Buffer->m_IsRelatedToPanel = gui.m_IsRelatedToMainPanel;
	m_UIWrapper.m_Buffer->m_RelatedPanelPosition = relatedPanelPosition;
	m_UIWrapper.m_Buffer->m_RelatedPanelSize = relatedPanelSize;
	m_UIWrapper.m_Buffer++;

	m_UIWrapper.m_Buffer->m_Vertex = glm::vec2((character->m_Mesh2D[12] * character->m_Scale.x + character->m_Position.x), ((character->m_Mesh2D[13]) * character->m_Scale.y + character->m_Position.y));
	m_UIWrapper.m_Buffer->m_UV = glm::vec2(character->m_Mesh2D[14], character->m_Mesh2D[15]);
	m_UIWrapper.m_Buffer->m_Static = IsStatic;
	m_UIWrapper.m_Buffer->m_Color = character->m_Color;
	m_UIWrapper.m_Buffer->m_IsGui = 0.0f;
	m_UIWrapper.m_Buffer->m_TexIndex = m_UIWrapper.m_TextureIndex;
	m_UIWrapper.m_Buffer->m_IsRelatedToPanel = gui.m_IsRelatedToMainPanel;
	m_UIWrapper.m_Buffer->m_RelatedPanelPosition = relatedPanelPosition;
	m_UIWrapper.m_Buffer->m_RelatedPanelSize = relatedPanelSize;
	m_UIWrapper.m_Buffer++;

	m_UIWrapper.m_IndexCount += 6;
}

void Batch::Submit(float* mesh, const glm::vec4& color, const glm::vec2& position, const glm::vec2& size, Texture* texture,
	std::vector<float> uv)
{
	if (m_UIWrapper.m_TextureSlotIndex > m_MaxTextureSlots - 1 || m_UIWrapper.m_IndexCount >= RENDERER_INDICES_SIZE)
	{
		Batch::GetInstance().EndUI();
		Batch::GetInstance().FlushUI();
		Batch::GetInstance().BeginUI();
	}

	m_UIWrapper.m_TextureIndex = -1.0f;
	if (texture == nullptr)
	{
		texture = TextureManager::GetInstance().White();
	}

	for (unsigned int i = 0; i < m_MaxTextureSlots; i++)
	{
		if (m_UIWrapper.m_TextureSlots[i] == texture->GetRendererID())
		{
			m_UIWrapper.m_TextureIndex = (float)i;
			break;
		}

	}
	if (m_UIWrapper.m_TextureIndex == -1.0f)
	{
		m_UIWrapper.m_TextureIndex = (float)m_UIWrapper.m_TextureSlotIndex;
		m_UIWrapper.m_TextureSlots[m_UIWrapper.m_TextureSlotIndex] = texture->GetRendererID();
		m_UIWrapper.m_TextureSlotIndex++;
	}

	Gui& gui = Gui::GetInstance();

	float aspect = Viewport::GetInstance().GetSize().y / gui.m_RelativeHeight;
	
	glm::vec2 viewportSize = (glm::vec2)Viewport::GetInstance().GetSize();
	glm::vec2 relatedPanelPosition = gui.m_CurrentPanelPosition * aspect + viewportSize * 0.5f;
	glm::vec2 positionOfRect = position * aspect + viewportSize * 0.5f;

	glm::vec2 relatedPanelSize = gui.m_CurrentPanelSize * aspect;
	glm::vec2 panelSize = size * aspect;

	if (uv.size() == 0)
	{
		uv = m_DefaultUV;
	}

	m_UIWrapper.m_Buffer->m_Vertex = glm::vec2((mesh[0]), mesh[1]);
	m_UIWrapper.m_Buffer->m_UV = glm::vec2(uv[0], uv[1]);
	m_UIWrapper.m_Buffer->m_Static = true;
	m_UIWrapper.m_Buffer->m_Color = color;
	m_UIWrapper.m_Buffer->m_IsGui = 1.;
	m_UIWrapper.m_Buffer->m_TexIndex = m_UIWrapper.m_TextureIndex;
	m_UIWrapper.m_Buffer->m_IsRelatedToPanel = gui.m_IsRelatedToMainPanel;
	m_UIWrapper.m_Buffer->m_RelatedPanelPosition = relatedPanelPosition;
	m_UIWrapper.m_Buffer->m_RelatedPanelSize = relatedPanelSize;
	m_UIWrapper.m_Buffer->m_RoundedCorners = glm::vec4(gui.m_Theme.m_RoundedCorners) * 2.0f;
	m_UIWrapper.m_Buffer->m_PreviousRoundedCorners = glm::vec4(gui.m_Theme.m_PreviousRoundedCorners) * 2.0f;
	m_UIWrapper.m_Buffer->m_Position = positionOfRect;
	m_UIWrapper.m_Buffer->m_Size = panelSize;
	m_UIWrapper.m_Buffer++;

	m_UIWrapper.m_Buffer->m_Vertex = glm::vec2((mesh[2]), (mesh[3]));
	m_UIWrapper.m_Buffer->m_UV = glm::vec2(uv[2], uv[3]);
	m_UIWrapper.m_Buffer->m_Static = true;
	m_UIWrapper.m_Buffer->m_Color = color;
	m_UIWrapper.m_Buffer->m_IsGui = 1.;
	m_UIWrapper.m_Buffer->m_TexIndex = m_UIWrapper.m_TextureIndex;
	m_UIWrapper.m_Buffer->m_IsRelatedToPanel = gui.m_IsRelatedToMainPanel;
	m_UIWrapper.m_Buffer->m_RelatedPanelPosition = relatedPanelPosition;
	m_UIWrapper.m_Buffer->m_RelatedPanelSize = relatedPanelSize;
	m_UIWrapper.m_Buffer->m_RoundedCorners = glm::vec4(gui.m_Theme.m_RoundedCorners) * 2.0f;
	m_UIWrapper.m_Buffer->m_PreviousRoundedCorners = glm::vec4(gui.m_Theme.m_PreviousRoundedCorners) * 2.0f;
	m_UIWrapper.m_Buffer->m_Position = positionOfRect;
	m_UIWrapper.m_Buffer->m_Size = panelSize;
	m_UIWrapper.m_Buffer++;

	m_UIWrapper.m_Buffer->m_Vertex = glm::vec2((mesh[4]), (mesh[5]));
	m_UIWrapper.m_Buffer->m_UV = glm::vec2(uv[4], uv[5]);
	m_UIWrapper.m_Buffer->m_Static = true;
	m_UIWrapper.m_Buffer->m_Color = color;
	m_UIWrapper.m_Buffer->m_IsGui = 1.;
	m_UIWrapper.m_Buffer->m_TexIndex = m_UIWrapper.m_TextureIndex;
	m_UIWrapper.m_Buffer->m_IsRelatedToPanel = gui.m_IsRelatedToMainPanel;
	m_UIWrapper.m_Buffer->m_RelatedPanelPosition = relatedPanelPosition;
	m_UIWrapper.m_Buffer->m_RelatedPanelSize = relatedPanelSize;
	m_UIWrapper.m_Buffer->m_RoundedCorners = glm::vec4(gui.m_Theme.m_RoundedCorners) * 2.0f;
	m_UIWrapper.m_Buffer->m_PreviousRoundedCorners = glm::vec4(gui.m_Theme.m_PreviousRoundedCorners) * 2.0f;
	m_UIWrapper.m_Buffer->m_Position = positionOfRect;
	m_UIWrapper.m_Buffer->m_Size = panelSize;
	m_UIWrapper.m_Buffer++;

	m_UIWrapper.m_Buffer->m_Vertex = glm::vec2((mesh[6]), ((mesh[7])));
	m_UIWrapper.m_Buffer->m_UV = glm::vec2(uv[6], uv[7]);
	m_UIWrapper.m_Buffer->m_Static = true;
	m_UIWrapper.m_Buffer->m_Color = color;
	m_UIWrapper.m_Buffer->m_IsGui = 1.;
	m_UIWrapper.m_Buffer->m_TexIndex = m_UIWrapper.m_TextureIndex;
	m_UIWrapper.m_Buffer->m_IsRelatedToPanel = gui.m_IsRelatedToMainPanel;
	m_UIWrapper.m_Buffer->m_RelatedPanelPosition = relatedPanelPosition;
	m_UIWrapper.m_Buffer->m_RelatedPanelSize = relatedPanelSize;
	m_UIWrapper.m_Buffer->m_RoundedCorners = glm::vec4(gui.m_Theme.m_RoundedCorners) * 2.0f;
	m_UIWrapper.m_Buffer->m_PreviousRoundedCorners = glm::vec4(gui.m_Theme.m_PreviousRoundedCorners) * 2.0f;
	m_UIWrapper.m_Buffer->m_Position = positionOfRect;
	m_UIWrapper.m_Buffer->m_Size = panelSize;
	m_UIWrapper.m_Buffer++;

	m_UIWrapper.m_IndexCount += 6;
}

void Batch::Submit(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color)
{
	if (m_LineWrapper.m_IndexCount >= RENDERER_INDICES_SIZE_LINES)
	{
		Batch::GetInstance().EndLines();
		Batch::GetInstance().FlushLines();
		Batch::GetInstance().BeginLines();
	}

	m_LineWrapper.m_Buffer->m_Position = start;
	m_LineWrapper.m_Buffer->m_Color = color;
	m_LineWrapper.m_Buffer++;

	m_LineWrapper.m_Buffer->m_Position = end;
	m_LineWrapper.m_Buffer->m_Color = color;
	m_LineWrapper.m_Buffer++;

	m_LineWrapper.m_IndexCount += 2;

	Editor::GetInstance().m_Stats.m_Vertices += 2;
}

void Batch::Submit(const std::vector<float>& vertices, const std::vector<float>& uv, const glm::mat4& transform,
	const glm::vec4& color, std::vector<class Texture*> textures, const glm::vec4& additionalData0, const glm::vec4& additionalData1)
{
	if (m_GOWrapper.m_TextureSlotIndex > m_MaxTextureSlots - 1 || m_GOWrapper.m_IndexCount >= RENDERER_INDICES_SIZE)
	{
		Batch::GetInstance().EndGameObjects();
		Batch::GetInstance().FlushGameObjects();
		Batch::GetInstance().BeginGameObjects();
	}

	if (textures.size() > 0)
	{
		m_GOWrapper.m_TextureIndex = m_GOWrapper.GetTextureSlot(m_MaxTextureSlots, textures[0]);
	}

	if (textures.size() > 1)
	{
		m_GOWrapper.m_NormalTextureIndex = m_GOWrapper.GetTextureSlot(m_MaxTextureSlots, textures[1]);
	}

	if (textures.size() > 2)
	{
		m_GOWrapper.m_EmissiveMaskTextureIndex = m_GOWrapper.GetTextureSlot(m_MaxTextureSlots, textures[2]);
	}

	m_GOWrapper.m_Buffer->m_Position = glm::vec3(vertices[0], vertices[1], 0.0f);
	m_GOWrapper.m_Buffer->m_UV = glm::vec2(uv[0], uv[1]);
	m_GOWrapper.m_Buffer->m_Color = color;
	m_GOWrapper.m_Buffer->m_TransformMat0 = transform[0];
	m_GOWrapper.m_Buffer->m_TransformMat1 = transform[1];
	m_GOWrapper.m_Buffer->m_TransformMat2 = transform[2];
	m_GOWrapper.m_Buffer->m_TransformMat3 = transform[3];
	m_GOWrapper.m_Buffer->m_AdditionalData0 = additionalData0;
	m_GOWrapper.m_Buffer->m_AdditionalData1 = additionalData1;
	m_GOWrapper.m_Buffer->m_TextureData[0] = m_GOWrapper.m_TextureIndex;
	m_GOWrapper.m_Buffer->m_TextureData[1] = m_GOWrapper.m_NormalTextureIndex;
	m_GOWrapper.m_Buffer->m_TextureData[2] = m_GOWrapper.m_EmissiveMaskTextureIndex;
	m_GOWrapper.m_Buffer++;

	m_GOWrapper.m_Buffer->m_Position = glm::vec3(vertices[4], vertices[5], 0.0f);
	m_GOWrapper.m_Buffer->m_UV = glm::vec2(uv[2], uv[3]);
	m_GOWrapper.m_Buffer->m_Color = color;
	m_GOWrapper.m_Buffer->m_TransformMat0 = transform[0];
	m_GOWrapper.m_Buffer->m_TransformMat1 = transform[1];
	m_GOWrapper.m_Buffer->m_TransformMat2 = transform[2];
	m_GOWrapper.m_Buffer->m_TransformMat3 = transform[3];
	m_GOWrapper.m_Buffer->m_AdditionalData0 = additionalData0;
	m_GOWrapper.m_Buffer->m_AdditionalData1 = additionalData1;
	m_GOWrapper.m_Buffer->m_TextureData[0] = m_GOWrapper.m_TextureIndex;
	m_GOWrapper.m_Buffer->m_TextureData[1] = m_GOWrapper.m_NormalTextureIndex;
	m_GOWrapper.m_Buffer->m_TextureData[2] = m_GOWrapper.m_EmissiveMaskTextureIndex;
	m_GOWrapper.m_Buffer++;

	m_GOWrapper.m_Buffer->m_Position = glm::vec3(vertices[8], vertices[9], 0.0f);
	m_GOWrapper.m_Buffer->m_UV = glm::vec2(uv[4], uv[5]);
	m_GOWrapper.m_Buffer->m_Color = color;
	m_GOWrapper.m_Buffer->m_TransformMat0 = transform[0];
	m_GOWrapper.m_Buffer->m_TransformMat1 = transform[1];
	m_GOWrapper.m_Buffer->m_TransformMat2 = transform[2];
	m_GOWrapper.m_Buffer->m_TransformMat3 = transform[3];
	m_GOWrapper.m_Buffer->m_AdditionalData0 = additionalData0;
	m_GOWrapper.m_Buffer->m_AdditionalData1 = additionalData1;
	m_GOWrapper.m_Buffer->m_TextureData[0] = m_GOWrapper.m_TextureIndex;
	m_GOWrapper.m_Buffer->m_TextureData[1] = m_GOWrapper.m_NormalTextureIndex;
	m_GOWrapper.m_Buffer->m_TextureData[2] = m_GOWrapper.m_EmissiveMaskTextureIndex;
	m_GOWrapper.m_Buffer++;

	m_GOWrapper.m_Buffer->m_Position = glm::vec3(vertices[12], vertices[13], 0.0f);
	m_GOWrapper.m_Buffer->m_UV = glm::vec2(uv[6], uv[7]);
	m_GOWrapper.m_Buffer->m_Color = color;
	m_GOWrapper.m_Buffer->m_TransformMat0 = transform[0];
	m_GOWrapper.m_Buffer->m_TransformMat1 = transform[1];
	m_GOWrapper.m_Buffer->m_TransformMat2 = transform[2];
	m_GOWrapper.m_Buffer->m_TransformMat3 = transform[3];
	m_GOWrapper.m_Buffer->m_AdditionalData0 = additionalData0;
	m_GOWrapper.m_Buffer->m_AdditionalData1 = additionalData1;
	m_GOWrapper.m_Buffer->m_TextureData[0] = m_GOWrapper.m_TextureIndex;
	m_GOWrapper.m_Buffer->m_TextureData[1] = m_GOWrapper.m_NormalTextureIndex;
	m_GOWrapper.m_Buffer->m_TextureData[2] = m_GOWrapper.m_EmissiveMaskTextureIndex;
	m_GOWrapper.m_Buffer++;

	m_GOWrapper.m_IndexCount += 6;

	Editor::GetInstance().m_Stats.m_Vertices += 4;
}

void Batch::Submit(const std::vector<float>& mesh, const glm::mat4& transform, const glm::vec4& color, std::vector<class Texture*> textures,
	const glm::vec4& additionalData)
{
	if (m_GOWrapper.m_TextureSlotIndex > m_MaxTextureSlots - 1 || m_GOWrapper.m_IndexCount >= RENDERER_INDICES_SIZE)
	{
		Batch::GetInstance().EndGameObjects();
		Batch::GetInstance().FlushGameObjects();
		Batch::GetInstance().BeginGameObjects();
	}

	if (textures.size() > 0)
	{
		m_GOWrapper.m_TextureIndex = m_GOWrapper.GetTextureSlot(m_MaxTextureSlots, textures[0]);
	}

	if (textures.size() == 2)
	{
		m_GOWrapper.m_NormalTextureIndex = m_GOWrapper.GetTextureSlot(m_MaxTextureSlots, textures[1]);
	}

	m_GOWrapper.m_Buffer->m_Position = glm::vec3(mesh[0], mesh[1], 0.0f);
	m_GOWrapper.m_Buffer->m_UV = glm::vec2(mesh[2], mesh[3]);
	m_GOWrapper.m_Buffer->m_Color = color;
	m_GOWrapper.m_Buffer->m_TransformMat0 = transform[0];
	m_GOWrapper.m_Buffer->m_TransformMat1 = transform[1];
	m_GOWrapper.m_Buffer->m_TransformMat2 = transform[2];
	m_GOWrapper.m_Buffer->m_TransformMat3 = transform[3];
	m_GOWrapper.m_Buffer->m_AdditionalData0 = additionalData;
	m_GOWrapper.m_Buffer->m_TextureData[0] = m_GOWrapper.m_TextureIndex;
	m_GOWrapper.m_Buffer->m_TextureData[1] = m_GOWrapper.m_NormalTextureIndex;
	m_GOWrapper.m_Buffer++;

	m_GOWrapper.m_Buffer->m_Position = glm::vec3(mesh[4], mesh[5], 0.0f);
	m_GOWrapper.m_Buffer->m_UV = glm::vec2(mesh[6], mesh[7]);
	m_GOWrapper.m_Buffer->m_Color = color;
	m_GOWrapper.m_Buffer->m_TransformMat0 = transform[0];
	m_GOWrapper.m_Buffer->m_TransformMat1 = transform[1];
	m_GOWrapper.m_Buffer->m_TransformMat2 = transform[2];
	m_GOWrapper.m_Buffer->m_TransformMat3 = transform[3];
	m_GOWrapper.m_Buffer->m_AdditionalData0 = additionalData;
	m_GOWrapper.m_Buffer->m_TextureData[0] = m_GOWrapper.m_TextureIndex;
	m_GOWrapper.m_Buffer->m_TextureData[1] = m_GOWrapper.m_NormalTextureIndex;
	m_GOWrapper.m_Buffer++;

	m_GOWrapper.m_Buffer->m_Position = glm::vec3(mesh[8], mesh[9], 0.0f);
	m_GOWrapper.m_Buffer->m_UV = glm::vec2(mesh[10], mesh[11]);
	m_GOWrapper.m_Buffer->m_Color = color;
	m_GOWrapper.m_Buffer->m_TransformMat0 = transform[0];
	m_GOWrapper.m_Buffer->m_TransformMat1 = transform[1];
	m_GOWrapper.m_Buffer->m_TransformMat2 = transform[2];
	m_GOWrapper.m_Buffer->m_TransformMat3 = transform[3];
	m_GOWrapper.m_Buffer->m_AdditionalData0 = additionalData;
	m_GOWrapper.m_Buffer->m_TextureData[0] = m_GOWrapper.m_TextureIndex;
	m_GOWrapper.m_Buffer->m_TextureData[1] = m_GOWrapper.m_NormalTextureIndex;
	m_GOWrapper.m_Buffer++;

	m_GOWrapper.m_Buffer->m_Position = glm::vec3(mesh[12], mesh[13], 0.0f);
	m_GOWrapper.m_Buffer->m_UV = glm::vec2(mesh[14], mesh[15]);
	m_GOWrapper.m_Buffer->m_Color = color;
	m_GOWrapper.m_Buffer->m_TransformMat0 = transform[0];
	m_GOWrapper.m_Buffer->m_TransformMat1 = transform[1];
	m_GOWrapper.m_Buffer->m_TransformMat2 = transform[2];
	m_GOWrapper.m_Buffer->m_TransformMat3 = transform[3];
	m_GOWrapper.m_Buffer->m_AdditionalData0 = additionalData;
	m_GOWrapper.m_Buffer->m_TextureData[0] = m_GOWrapper.m_TextureIndex;
	m_GOWrapper.m_Buffer->m_TextureData[1] = m_GOWrapper.m_NormalTextureIndex;
	m_GOWrapper.m_Buffer++;

	m_GOWrapper.m_IndexCount += 6;

	Editor::GetInstance().m_Stats.m_Vertices += 4;
}

void Batch::Submit(const std::vector<float>& mesh, const glm::mat4& positionMat4, const glm::mat4& rotationMat4,
	const glm::mat4& scaleMat4, const glm::vec4& color, std::vector<class Texture*> textures)
{
	glm::mat4 transform = positionMat4 * rotationMat4 * scaleMat4;
	Submit(mesh, transform, color, textures);
}

void Batch::FlushGameObjects()
{
	glBindVertexArray(m_GOWrapper.m_Vao);
	m_GOWrapper.m_Ibo.Bind();

	Shader* shader = Shader::Get("Default2D");
	shader->Bind();
	int samplers[32];
	for (unsigned int i = m_GOWrapper.m_StartTextureSlotIndex; i < m_GOWrapper.m_TextureSlotIndex; i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, m_GOWrapper.m_TextureSlots[i]);
	}
	for (unsigned int i = 0; i < m_MaxTextureSlots; i++)
	{
		samplers[i] = i;
	}

	shader->SetUniform1f("u_GlobalIntensity", Environment::GetInstance().GetGlobalIntensity());
	shader->SetUniform1iv("u_Texture", samplers);
	shader->SetUniformMat4f("u_ViewProjectionMat4", Environment::GetInstance().GetMainCamera()->GetViewProjectionMat4());
	//shader->SetUniform2fv("u_Resolution", Viewport::GetInstance().GetSize());

	char buffer[64];
	Scene* scene = EntryPoint::GetApplication().GetScene();
	int pointLightSize = scene->m_PointLights2D.size();
	shader->SetUniform1i("pointLightSize", pointLightSize);
	for (int i = 0; i < pointLightSize; i++)
	{
		PointLight2D* pointLight2D = scene->m_PointLights2D[i];
		sprintf(buffer, "pointLights[%i].position", i);
		shader->SetUniform3fv(buffer, pointLight2D->GetOwner()->m_Transform.GetPosition());
		sprintf(buffer, "pointLights[%i].color", i);
		shader->SetUniform3fv(buffer, pointLight2D->m_Color);
		sprintf(buffer, "pointLights[%i].constant", i);
		shader->SetUniform1f(buffer, pointLight2D->m_Constant);
		sprintf(buffer, "pointLights[%i]._linear", i);
		shader->SetUniform1f(buffer, pointLight2D->m_Linear);
		sprintf(buffer, "pointLights[%i].quadratic", i);
		shader->SetUniform1f(buffer, pointLight2D->m_Quadratic);
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDrawElements(GL_TRIANGLES, m_GOWrapper.m_IndexCount, GL_UNSIGNED_INT, NULL);

	Editor::GetInstance().m_Stats.m_DrawCalls++;
	Editor::GetInstance().m_Stats.m_Indices += m_GOWrapper.m_IndexCount;

	shader->UnBind();
	m_GOWrapper.m_Ibo.UnBind();
	m_GOWrapper.m_TextureSlotIndex = m_GOWrapper.m_StartTextureSlotIndex;
	m_GOWrapper.m_IndexCount = 0;
	for (unsigned int i = m_GOWrapper.m_TextureSlotIndex; i < 32; i++)
	{
		m_GOWrapper.m_TextureSlots[i] = 0;
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	glDisable(GL_TEXTURE_2D_ARRAY);
}

void Batch::FlushLines()
{
	Window::GetInstance().Clear({ 0.0f, 0.0f, 0.0f, 0.0f });

	glBindVertexArray(m_LineWrapper.m_Vao);
	m_LineWrapper.m_Ibo.Bind();

	Shader* shader = Shader::Get("DefaultLines");
	shader->Bind();
	shader->SetUniformMat4f("u_ViewProjectionMat4", Environment::GetInstance().GetMainCamera()->GetViewProjectionMat4());

	glLineWidth(Editor::GetInstance().m_LineWidth);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDrawElements(GL_LINES, m_LineWrapper.m_IndexCount, GL_UNSIGNED_INT, NULL);
	
	Editor::GetInstance().m_Stats.m_DrawCalls++;
	Editor::GetInstance().m_Stats.m_Indices += m_LineWrapper.m_IndexCount;

	shader->UnBind();
	m_LineWrapper.m_Ibo.UnBind();
	m_LineWrapper.m_IndexCount = 0;
}

void Batch::FlushUI()
{
	glBindVertexArray(m_UIWrapper.m_Vao);
	m_UIWrapper.m_Ibo.Bind();
	Shader* shader = Shader::Get("UI");
	shader->Bind();

	int samplers[32];
	for (unsigned int i = 0; i < m_UIWrapper.m_TextureSlotIndex; i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, m_UIWrapper.m_TextureSlots[i]);
	}
	for (unsigned int i = 0; i < m_MaxTextureSlots; i++)
	{
		samplers[i] = i;
	}

	Gui& gui = Gui::GetInstance();
	shader->SetUniform1iv("u_Texture", samplers);
	shader->SetUniformMat4f("u_Projection", gui.m_ViewProjection);
	//shader->SetUniformMat4f("u_ViewProjection", Environment::GetInstance().GetMainCamera()->GetViewProjectionMat4());
	shader->SetUniform2fv("u_offset", gui.m_TextProps.m_ShadowOffset);
	shader->SetUniform1f("u_width", gui.m_TextProps.m_Width);
	shader->SetUniform1f("u_edge", gui.m_TextProps.m_Edge);
	shader->SetUniform1f("u_borderwidth", gui.m_TextProps.m_BorderWidth);
	shader->SetUniform1f("u_borderedge", gui.m_TextProps.m_BorderEdge);
	shader->SetUniform1f("u_RoundedRadius", gui.m_Theme.m_RoundedRadius);
	shader->SetUniform4fv("u_outlineColor", gui.m_TextProps.m_OutLineColor);
	//shader->SetUniform2fv("u_ViewPortSize", Viewport::GetInstance().GetSize());

	glDrawElements(GL_TRIANGLES, m_UIWrapper.m_IndexCount, GL_UNSIGNED_INT, NULL);

	Editor::GetInstance().m_Stats.m_DrawCalls++;
	Editor::GetInstance().m_Stats.m_Indices += m_UIWrapper.m_IndexCount;

	shader->UnBind();
	m_UIWrapper.m_Ibo.UnBind();
	m_UIWrapper.m_TextureSlotIndex = m_UIWrapper.m_StartTextureSlotIndex;
	m_UIWrapper.m_IndexCount = 0;
	for (unsigned int i = m_UIWrapper.m_TextureSlotIndex; i < 32; i++)
	{
		m_UIWrapper.m_TextureSlots[i] = 0;
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	glDisable(GL_TEXTURE_2D_ARRAY);
}

void Batch::BeginLines()
{
	m_LineWrapper.m_Buffer = m_LineWrapper.m_BufferPtr;
	m_LineWrapper.m_IndexCount = 0;
}

void Batch::EndLines()
{
	uint32_t dataSize = (uint8_t*)m_LineWrapper.m_Buffer - (uint8_t*)m_LineWrapper.m_BufferPtr;
	glBindBuffer(GL_ARRAY_BUFFER, m_LineWrapper.m_Vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, dataSize, m_LineWrapper.m_BufferPtr);
	FlushLines();
}

void Batch::BeginGameObjects()
{
	m_GOWrapper.m_Buffer = m_GOWrapper.m_BufferPtr;
	m_GOWrapper.m_IndexCount = 0;
}

void Batch::EndGameObjects()
{
	uint32_t dataSize = (uint8_t*)m_GOWrapper.m_Buffer - (uint8_t*)m_GOWrapper.m_BufferPtr;
	glBindBuffer(GL_ARRAY_BUFFER, m_GOWrapper.m_Vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, dataSize, m_GOWrapper.m_BufferPtr);
	FlushGameObjects();
}

void Batch::BeginUI()
{
	m_UIWrapper.m_Buffer = m_UIWrapper.m_BufferPtr;
	m_UIWrapper.m_IndexCount = 0;
}

void Batch::EndUI()
{
	uint32_t dataSize = (uint8_t*)m_UIWrapper.m_Buffer - (uint8_t*)m_UIWrapper.m_BufferPtr;
	glBindBuffer(GL_ARRAY_BUFFER, m_UIWrapper.m_Vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, dataSize, m_UIWrapper.m_BufferPtr);
	FlushUI();
}
