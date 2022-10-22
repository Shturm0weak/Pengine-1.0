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
	Renderer3D& r3d = *(Renderer3D*)&component;
	SetMesh(r3d.m_Mesh);
    SetMaterial(r3d.m_Material->Inherit());
	SetOpaque(r3d.m_IsOpaque);
	SetDrawShadows(r3d.m_DrawShadows);
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

	glm::vec3 scale = { m_Material->m_Scale, m_Material->m_Scale, m_Material->m_Scale };

	m_Shader->SetUniform3fv("u_Material.ambient", m_Material->m_Ambient * scale);
	m_Shader->SetUniform3fv("u_Material.diffuse", m_Material->m_Diffuse * scale);
	m_Shader->SetUniform3fv("u_Material.specular", m_Material->m_Specular * scale);
	m_Shader->SetUniform1f("u_Material.shininess", m_Material->m_Shininess);
	m_Shader->SetUniform1f("u_Material.solid", m_Material->m_Solid);
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, m_Material->m_BaseColor->GetRendererID());
	m_Shader->SetUniform1i("u_Material.baseColor", 0);

	if (GetOwner()->GetScene()->m_DirectionalLights.empty())
	{
		m_Shader->SetUniform1f("u_DirectionalLight.intensity", 0.0f);
	}
	else
	{
		DirectionalLight* directionalLight = GetOwner()->GetScene()->m_DirectionalLights[0];

		m_Shader->SetUniform3fv("u_DirectionalLight.direction", directionalLight->GetOwner()->m_Transform.GetRotationMat4() * glm::vec4(0, 0, -1, 1));
		m_Shader->SetUniform3fv("u_DirectionalLight.color", directionalLight->m_Color);
		m_Shader->SetUniform1f("u_DirectionalLight.intensity", directionalLight->m_Intensity);
	}
	
	int pointLightsSize = GetOwner()->GetScene()->m_PointLights.size();
	m_Shader->SetUniform1i("pointLightsSize", pointLightsSize);
	char uniformBuffer[64];
	for (int i = 0; i < pointLightsSize; i++)
	{
		PointLight* pointLight = GetOwner()->GetScene()->m_PointLights[i];
		sprintf(uniformBuffer, "pointLights[%i].position", i);
		m_Shader->SetUniform3fv(uniformBuffer, pointLight->GetOwner()->m_Transform.GetPosition());
		sprintf(uniformBuffer, "pointLights[%i].color", i);
		m_Shader->SetUniform3fv(uniformBuffer, pointLight->m_Color);
		sprintf(uniformBuffer, "pointLights[%i].constant", i);
		m_Shader->SetUniform1f(uniformBuffer, pointLight->m_Constant);
		sprintf(uniformBuffer, "pointLights[%i]._linear", i);
		m_Shader->SetUniform1f(uniformBuffer, pointLight->m_Linear);
		sprintf(uniformBuffer, "pointLights[%i].quadratic", i);
		m_Shader->SetUniform1f(uniformBuffer, pointLight->m_Quadratic);
	}

	int spotLightsSize = GetOwner()->GetScene()->m_SpotLights.size();
	m_Shader->SetUniform1i("spotLightsSize", spotLightsSize);
	for (int i = 0; i < spotLightsSize; i++)
	{
		SpotLight* spotLight = GetOwner()->GetScene()->m_SpotLights[i];
		sprintf(uniformBuffer, "spotLights[%i].position", i);
		m_Shader->SetUniform3fv(uniformBuffer, spotLight->GetOwner()->m_Transform.GetPosition());
		sprintf(uniformBuffer, "spotLights[%i].direction", i);
		m_Shader->SetUniform3fv(uniformBuffer, spotLight->GetDirection());
		sprintf(uniformBuffer, "spotLights[%i].color", i);
		m_Shader->SetUniform3fv(uniformBuffer, spotLight->m_Color);
		sprintf(uniformBuffer, "spotLights[%i].constant", i);
		m_Shader->SetUniform1f(uniformBuffer, spotLight->m_Constant);
		sprintf(uniformBuffer, "spotLights[%i]._linear", i);
		m_Shader->SetUniform1f(uniformBuffer, spotLight->m_Linear);
		sprintf(uniformBuffer, "spotLights[%i].quadratic", i);
		m_Shader->SetUniform1f(uniformBuffer, spotLight->m_Quadratic);
		sprintf(uniformBuffer, "spotLights[%i].innerCutOff", i);
		m_Shader->SetUniform1f(uniformBuffer, (spotLight->m_CosInnerCutOff));
		sprintf(uniformBuffer, "spotLights[%i].outerCutOff", i);
		m_Shader->SetUniform1f(uniformBuffer, (spotLight->m_CosOuterCutOff));
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

	auto opaqueByMesh = scene->m_OpaqueByMesh.find(m_Mesh);
	if (opaqueByMesh != scene->m_OpaqueByMesh.end())
	{
		Utils::Erase<Renderer3D>(opaqueByMesh->second, this);
	}
}

void Renderer3D::AddToInstancing()
{
	Scene* scene = GetOwner()->GetScene();

	auto opaqueByMesh = scene->m_OpaqueByMesh.find(m_Mesh);
	if (opaqueByMesh != scene->m_OpaqueByMesh.end())
	{
		if (!Utils::IsThere<Renderer3D*>(opaqueByMesh->second, this))
		{
			opaqueByMesh->second.push_back(this);
		}
	}
	else
	{
		scene->m_OpaqueByMesh.emplace(m_Mesh, std::vector<Renderer3D*>() = { this });
	}
}

void Renderer3D::EraseFromTransparent()
{
	Scene* scene = GetOwner()->GetScene();

	if(Utils::Erase<Renderer3D*>(scene->m_TransparentByDistance, this))
	{
		return;
	}
}

void Renderer3D::AddToTransparent()
{
	std::shared_ptr<Camera> camera = Environment::GetInstance().GetMainCamera();
	Scene* scene = GetOwner()->GetScene();

	if (Utils::IsThere<Renderer3D*>(scene->m_TransparentByDistance, this))
	{
		return;
	}

	scene->m_TransparentByDistance.emplace_back(this);
}

void Renderer3D::EraseFromShadows()
{
	Scene* scene = GetOwner()->GetScene();

	auto shadowsByMesh = scene->m_ShadowsByMesh.find(m_Mesh);
	if (shadowsByMesh != scene->m_ShadowsByMesh.end())
	{
		Utils::Erase<Renderer3D>(shadowsByMesh->second, this);
	}
}

void Renderer3D::AddToShadows()
{
	Scene* scene = GetOwner()->GetScene();

	auto shadowsByMesh = scene->m_ShadowsByMesh.find(m_Mesh);
	if (shadowsByMesh != scene->m_ShadowsByMesh.end())
	{
		if (!Utils::IsThere<Renderer3D*>(shadowsByMesh->second, this))
		{
			shadowsByMesh->second.push_back(this);
		}
	}
	else
	{
		scene->m_ShadowsByMesh.emplace(m_Mesh, std::vector<Renderer3D*>() = { this });
	}
}

IComponent* Renderer3D::Create(GameObject* owner)
{
	Renderer3D* r3d = MemoryManager::GetInstance().Allocate<Renderer3D>();
	r3d->m_Owner = owner;
	r3d->m_Type = Utils::GetTypeName<Renderer3D>();
	r3d->SetMaterial(MaterialManager::GetInstance().Load("Source/Materials/Material.mat"));
	owner->GetScene()->m_Renderers3D.push_back(r3d);

	return r3d;
}

void Renderer3D::Delete()
{
	SetMaterial(MaterialManager::GetInstance().Load("Source/Materials/Material.mat"));
	
	if (m_IsOpaque)
	{
		EraseFromInstancing();
	}
	else
	{
		EraseFromTransparent();
	}

	EraseFromShadows();

	Utils::Erase<Renderer3D>(GetOwner()->GetScene()->m_Renderers3D, this);
	MemoryManager::GetInstance().FreeMemory<Renderer3D>(static_cast<IAllocator*>(this));
}

void Renderer3D::SetMesh(Mesh* mesh, size_t lod)
{
	if (lod >= m_Lods.size())
	{
		lod = 0;
	}

	if (m_Mesh)
	{
		if (m_IsOpaque)
		{
			EraseFromInstancing();
		}
		else
		{
			EraseFromTransparent();
		}
	}

	if (mesh)
	{
		m_Mesh = mesh;
		m_Lods[lod] = m_Mesh;

		if (m_IsOpaque)
		{
			AddToInstancing();
		}
		else
		{
			AddToTransparent();
		}

		SetDrawShadows(m_DrawShadows);
	}
	else
	{
		EraseFromShadows();

		m_Mesh = mesh;
		m_Lods[lod] = m_Mesh;
	}
}

void Renderer3D::SetOpaque(bool isOpaque)
{
	if (!m_Mesh && isOpaque == m_IsOpaque)
	{
		return;
	}

	if (m_IsOpaque)
	{
		EraseFromInstancing();
	}
	else
	{
		EraseFromTransparent();
	}

	m_IsOpaque = isOpaque;

	if (m_IsOpaque)
	{
		AddToInstancing();
	}
	else
	{
		AddToTransparent();
	}
}

void Renderer3D::SetMaterial(Material* material)
{
	if (m_Material && m_Material->IsInherited())
	{
		delete m_Material;
	}

	m_Material = material;
}

void Renderer3D::SetDrawShadows(bool drawShadows)
{
	if (!m_Mesh && drawShadows == m_DrawShadows)
	{
		return;
	}

	m_DrawShadows = drawShadows;

	if (m_DrawShadows)
	{
		AddToShadows();
	}
	else
	{
		EraseFromShadows();
	}
}

void Renderer3D::SetCurrentLod(size_t lod)
{
	if (lod >= m_Lods.size())
	{
		lod = 0;
	}

	if (m_CurrentLOD == lod)
	{
		return;
	}

	m_CurrentLOD = lod;

	SetMesh(m_Lods[lod], lod);
}
