#include "Renderer3D.h"

#include "../Core/GameObject.h"
#include "../Core/Environment.h"
#include "../Core/MemoryManager.h"
#include "../Core/Editor.h"
#include "../Core/Scene.h"
#include "../OpenGL/Batch.h"
#include "../Core/Utils.h"
#include "../EventSystem/EventSystem.h"
#include "../Events/OnMainThreadCallback.h"

using namespace Pengine;

void Renderer3D::Copy(const IComponent& component)
{
	Renderer3D& r2d = *(Renderer3D*)&component;
	m_Mesh = r2d.m_Mesh;
	m_Material = r2d.m_Material;
	m_Type = component.GetType();
}

IComponent* Renderer3D::New(GameObject* owner)
{
	return Create(owner);
}

void Renderer3D::Render()
{
	if (!m_Mesh) return;

	m_Shader = Shader::Get("Default3D");
	m_Shader->Bind();
	m_Shader->SetUniformMat4f("u_ViewProjection", Environment::GetInstance().GetMainCamera()->GetViewProjectionMat4());
	m_Shader->SetUniformMat4f("u_Transform", m_Owner->m_Transform.GetTransform());
	m_Shader->SetUniform4fv("u_Color", m_Material.m_Color);
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, m_Material.m_Texture->GetRendererID());
	m_Shader->SetUniform1i("u_Diffuse", 0);

	m_Mesh->m_Va.Bind();
	m_Mesh->m_Ib.Bind();
	
	Editor::GetInstance().m_Stats.m_Vertices += (float)m_Mesh->GetVertexAttributes().size() / (float)m_Mesh->m_Layout.GetElements().size();
	Editor::GetInstance().m_Stats.m_Indices += m_Mesh->m_Ib.GetCount();
	Editor::GetInstance().m_Stats.m_DrawCalls++;
	
	if (Editor::GetInstance().m_PolygonMode)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	glDrawElements(GL_TRIANGLES, m_Mesh->m_Ib.GetCount(), GL_UNSIGNED_INT, nullptr);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	m_Shader->UnBind();

	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, TextureManager::GetInstance().White()->GetRendererID());
}

IComponent* Renderer3D::Create(GameObject* owner)
{
	Renderer3D* r2d = new Renderer3D();
	r2d->m_Type = Utils::GetTypeName<Renderer3D>();

	if (Utils::IsThere<IRenderer*>(owner->GetScene()->m_Renderers3D, r2d) == false)
	{
		owner->GetScene()->m_Renderers3D.push_back(r2d);
	}

	return r2d;
}

void Renderer3D::Delete()
{
	Utils::Erase<IRenderer>(m_Owner->GetScene()->m_Renderers3D, this);
	delete this;
}