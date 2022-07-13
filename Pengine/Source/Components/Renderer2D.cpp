#include "Renderer2D.h"

#include "../Core/GameObject.h"
#include "../Core/Environment.h"
#include "../Core/MemoryManager.h"
#include "../Core/Scene.h"
#include "../OpenGL/Batch.h"
#include "../Core/Utils.h"
#include "../EventSystem/EventSystem.h"
#include "../Events/OnMainThreadCallback.h"

using namespace Pengine;

void Renderer2D::Copy(const IComponent& component)
{
	Renderer2D& r2d = *(Renderer2D*)&component;
	m_Mesh = r2d.m_Mesh;
	m_Texture = r2d.m_Texture;
	m_NormalTexture = r2d.m_NormalTexture;
	m_EmissiveMaskTexture = r2d.m_EmissiveMaskTexture;
	m_Color = r2d.m_Color;
	m_Type = component.GetType();
	m_UV = r2d.m_UV;
	m_InnerRadius = r2d.m_InnerRadius;
	m_OuterRadius = r2d.m_OuterRadius;
	m_IsNormalUsed = r2d.m_IsNormalUsed;
	m_Ambient = r2d.m_Ambient;
	m_EmmisiveMaskIntensity = r2d.m_EmmisiveMaskIntensity;

	SetLayer(r2d.GetLayer());
}

IComponent* Renderer2D::New(GameObject* owner)
{
	return Create(owner);
}

void Renderer2D::Render()
{
	Batch::GetInstance().Submit(m_Mesh->GetVertexAttributes(), m_UV,
		m_Owner->m_Transform.GetTransform(), m_Color, { m_Texture, m_NormalTexture, m_EmissiveMaskTexture },
		{ m_InnerRadius, m_OuterRadius, (float)m_IsNormalUsed, m_Ambient },
		{ m_EmmisiveMaskIntensity, 0.0f, 0.0f, 0.0f });
}

IComponent* Renderer2D::Create(GameObject* owner)
{
	Renderer2D* r2d = MemoryManager::GetInstance().Allocate<Renderer2D>();
	r2d->m_Type = Utils::GetTypeName<Renderer2D>();
	r2d->SetLayer(0);

	return r2d;
}

void Renderer2D::Delete()
{
	Utils::Erase<Renderer2D>(m_Owner->GetScene()->m_Renderer2DLayers[m_Layer], this);
	MemoryManager::GetInstance().FreeMemory<Renderer2D>(static_cast<IAllocator*>(this));
}

void Renderer2D::SetTexture(Texture* texture)
{
	if (texture)
	{
		m_Texture = texture;
	}
}

void Renderer2D::SetNormalTexture(Texture* normalTexture)
{
	if (normalTexture)
	{
		m_NormalTexture = normalTexture;
	}
}

void Renderer2D::SetEmissiveMaskTexture(Texture* emissiveMaskTexture)
{
	if (emissiveMaskTexture)
	{
		m_EmissiveMaskTexture = emissiveMaskTexture;
	}
}

void Renderer2D::Flip()
{
	float temp0 = m_UV[0];
	float temp2 = m_UV[2];
	m_UV[0] = m_UV[4];
	m_UV[2] = m_UV[6];
	m_UV[4] = temp0;
	m_UV[6] = temp2;
}

void Renderer2D::ReversedUV()
{
	m_UV[0] = 1.0f;
	m_UV[1] = 0.0f;
	m_UV[2] = 0.0f;
	m_UV[3] = 0.0f;
	m_UV[4] = 0.0f;
	m_UV[5] = 1.0f;
	m_UV[6] = 1.0f;
	m_UV[7] = 1.0f;
}

void Renderer2D::OriginalUV()
{
	m_UV[0] = 0.0f;
	m_UV[1] = 0.0f;
	m_UV[2] = 1.0f;
	m_UV[3] = 0.0f;
	m_UV[4] = 1.0f;
	m_UV[5] = 1.0f;
	m_UV[6] = 0.0f;
	m_UV[7] = 1.0f;
}

void Renderer2D::SetLayer(int layer)
{
	if (layer > -1)
	{
		std::function<void()> callback = std::function<void()>([=] {
			Scene* scene = m_Owner->GetScene();
			Utils::Erase<Renderer2D>(scene->m_Renderer2DLayers[m_Layer], this);
			m_Layer = layer;
			scene->m_Renderer2DLayers[m_Layer].push_back(this);
		});
		EventSystem::GetInstance().SendEvent(new OnMainThreadCallback(callback, EventType::ONMAINTHREADPROCESS));
	}
}

void Renderer2D::SetUV(const std::vector<float>& uv)
{
	if (uv.size() == 8)
	{
		m_UV = uv;
	}
}

std::vector<float> Renderer2D::GetUV() const
{
	return m_UV;
}