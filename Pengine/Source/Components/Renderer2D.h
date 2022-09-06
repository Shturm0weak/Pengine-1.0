#pragma once

#include "../Core/Core.h"
#include "../Core/MeshManager.h"
#include "../Core/Utils.h"
#include "../Core/Allocator.h"
#include "../Core/TextureManager.h"
#include "IRenderer.h"

#include <map>

namespace Pengine
{

	class PENGINE_API Renderer2D : public IRenderer, public IAllocator
	{
	private:

		std::vector<float> m_UV =
		{
			0.0f, 0.0f,
			1.0f, 0.0f,
			1.0f, 1.0f,
			0.0f, 1.0f
		};

		Mesh* m_Mesh = MeshManager::GetInstance().Get("Quad");
		Texture* m_Texture = TextureManager::GetInstance().White();
		Texture* m_NormalTexture = TextureManager::GetInstance().White();
		Texture* m_EmissiveMaskTexture = TextureManager::GetInstance().White();

		glm::vec4 m_Color = glm::vec4(1.0f);
		
		int m_Layer = 0;

		bool m_IsNormalUsed = false;

		friend class Serializer;
		friend class Editor;
	public:

		float m_InnerRadius = 0.0f;
		float m_OuterRadius = 0.0f;

		float m_Ambient = 1.0f;
		float m_EmmisiveMaskIntensity = 0.0f;

		Renderer2D(int layer) { m_Layer = layer; }
		Renderer2D() = default;
		
		static IComponent* Create(GameObject* owner);

		virtual IComponent* New(GameObject* owner) override;
		
		virtual void Copy(const IComponent& component) override;
		
		virtual void Render() override;
		
		virtual void Delete() override;

		glm::vec4 GetColor() const { return m_Color; }
		
		void SetColor(const glm::vec4& color) { m_Color = color; }
		
		void SetTexture(Texture* texture);
		
		void SetNormalTexture(Texture* normalTexture);

		void SetEmissiveMaskTexture(Texture* emissiveMaskTexture);

		Texture* GetTexture() const { return m_Texture; }
		
		Texture* GetNormalTexture() const { return m_NormalTexture; }

		Texture* GetEmissiveMaskTexture() const { return m_EmissiveMaskTexture; }

		void SetShader(Shader* shader) { m_Shader = shader; }
		
		void Flip();
		
		void ReversedUV();
		
		void OriginalUV();
		
		void SetLayer(int layer);
		
		void SetUV(const std::vector<float>& uv);
		
		std::vector<float> GetUV() const;
		
		int GetLayer() const { return m_Layer; }
	};

}
