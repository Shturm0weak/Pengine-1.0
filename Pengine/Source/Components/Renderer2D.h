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
		Texture* m_Texture = TextureManager::GetInstance().Get("White");

		int m_Layer = 0;

		friend class Serializer;
	public:

		glm::vec4 m_Color = glm::vec4(1.0f);
		
		static IComponent* Create(GameObject* owner);

		virtual void Copy(const IComponent& component) override;
		virtual IComponent* Renderer2D::CreateCopy(GameObject* newOwner) override;
		virtual void Render() override;
		virtual void Delete() override;
		Renderer2D(int layer) { m_Layer = layer; }
		Renderer2D() = default;

		void operator=(const Renderer2D& renderer2D);
		void SetTexture(Texture* texture);
		void Flip();
		void ReversedUV();
		void OriginalUV();
		void SetLayer(int layer);
		void SetUV(std::vector<float> uv);
		int GetLayer() const { return m_Layer; }
		Texture* GetTexture() const { return m_Texture; }
	};

}
