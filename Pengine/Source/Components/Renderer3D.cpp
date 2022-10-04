#include "Renderer3D.h"

#include "../Core/GameObject.h"
#include "../Core/Environment.h"
#include "../Core/Editor.h"
#include "../Core/Scene.h"
#include "../Core/Utils.h"
#include "../Core/Instancing.h"
#include "../Core/MemoryManager.h"
#include "../Components/DirectionalLight.h"

using namespace Pengine;

void Renderer3D::Copy(const IComponent& component)
{
	Renderer3D& r2d = *(Renderer3D*)&component;
	SetMesh(r2d.m_Mesh);
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

	Environment& environment = Environment::GetInstance();

	m_Shader = Shader::Get("Default3D");
	m_Shader->Bind();
	m_Shader->SetUniformMat4f("u_ViewProjection", environment.GetMainCamera()->GetViewProjectionMat4());
	m_Shader->SetUniform3fv("u_CameraPosition", environment.GetMainCamera()->m_Transform.GetPosition());
	m_Shader->SetUniformMat4f("u_Transform", m_Owner->m_Transform.GetTransform());

	glm::vec3 scale = { m_Material.m_Scale, m_Material.m_Scale, m_Material.m_Scale };

	m_Shader->SetUniform3fv("u_Material.ambient", m_Material.m_Ambient * scale);
	m_Shader->SetUniform3fv("u_Material.diffuse", m_Material.m_Diffuse * scale);
	m_Shader->SetUniform3fv("u_Material.specular", m_Material.m_Specular * scale);
	m_Shader->SetUniform1f("u_Material.shininess", m_Material.m_Shininess);
	m_Shader->SetUniform1f("u_Material.solid", m_Material.m_Solid);
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, m_Material.m_BaseColor->GetRendererID());
	m_Shader->SetUniform1i("u_Material.baseColor", 0);

	if (GetOwner()->GetScene()->m_DirectionalLight.empty())
	{
		m_Shader->SetUniform1f("u_DirectionalLight.intensity", 0.0f);
	}
	else
	{
		DirectionalLight* directionalLight = GetOwner()->GetScene()->m_DirectionalLight[0];

		m_Shader->SetUniform3fv("u_DirectionalLight.direction", directionalLight->GetOwner()->m_Transform.GetRotationMat4() * glm::vec4(0, 0, -1, 1));
		m_Shader->SetUniform3fv("u_DirectionalLight.color", directionalLight->m_Color);
		m_Shader->SetUniform1f("u_DirectionalLight.intensity", directionalLight->m_Intensity);
	}
	
	m_Mesh->m_Va.Bind();
	m_Mesh->m_Ib.Bind();
	
	Editor::GetInstance().m_Stats.m_Vertices += (float)m_Mesh->GetVertexAttributes().size() / (float)m_Mesh->m_Layout.GetElements().size();
	Editor::GetInstance().m_Stats.m_Indices += m_Mesh->m_Ib.GetCount();
	Editor::GetInstance().m_Stats.m_DrawCalls++;

	if (Editor::GetInstance().m_PolygonMode)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	if (m_BackFaceCulling)
	{
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	}

	glDrawElements(GL_TRIANGLES, m_Mesh->m_Ib.GetCount(), GL_UNSIGNED_INT, nullptr);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	m_Shader->UnBind();

	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, TextureManager::GetInstance().White()->GetRendererID());
}

void Renderer3D::EraseFromInstancing()
{
	Scene* scene = GetOwner()->GetScene();
	for (auto i = scene->m_InstancedObjects.begin(); i != scene->m_InstancedObjects.end(); i++)
	{
		if (m_Mesh != nullptr)
		{
			Utils::Erase<Renderer3D>(i->second, this);
		}
	}
}

void Renderer3D::AddToInstancing()
{
	Scene* scene = GetOwner()->GetScene();

	if (scene->m_InstancedObjects.find(m_Mesh) == scene->m_InstancedObjects.end())
	{
		scene->m_InstancedObjects.emplace(m_Mesh, std::vector<Renderer3D*>());
	}

	for (auto i = scene->m_InstancedObjects.begin(); i != scene->m_InstancedObjects.end(); i++)
	{
		if (i->first == m_Mesh)
		{
			auto iterFind = std::find(i->second.begin(), i->second.end(), this);
			if (!Utils::IsThere<Renderer3D*>(i->second, this))
			{
				i->second.push_back(this);
			}
		}
	}
}

IComponent* Renderer3D::Create(GameObject* owner)
{
	Renderer3D* r3d = MemoryManager::GetInstance().Allocate<Renderer3D>();
	r3d->m_Owner = owner;
	r3d->m_Type = Utils::GetTypeName<Renderer3D>();

	return r3d;
}

void Renderer3D::Delete()
{
	EraseFromInstancing();
	MemoryManager::GetInstance().FreeMemory<Renderer3D>(static_cast<IAllocator*>(this));
}

void Renderer3D::SetMesh(Mesh* mesh)
{
	if (mesh == nullptr)
	{
		return;
	}

	EraseFromInstancing();

	m_Mesh = mesh;

	AddToInstancing();
}
