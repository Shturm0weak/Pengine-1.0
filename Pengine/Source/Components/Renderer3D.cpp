#include "Renderer3D.h"

#include "../Core/GameObject.h"
#include "../Core/Environment.h"
#include "../Core/Editor.h"
#include "../Core/Scene.h"
#include "../Core/Utils.h"
#include "../Core/Instancing.h"
#include "../Core/Renderer.h"
#include "../Core/MemoryManager.h"
#include "../Components/DirectionalLight.h"
#include "../OpenGL/Material.h"
#include "../OpenGL/FrameBuffer.h"

using namespace Pengine;

void Renderer3D::Copy(const IComponent& component)
{
	Renderer3D& r3d = *(Renderer3D*)&component;
    SetMaterial(r3d.m_Material);
	m_Lods = r3d.m_Lods;
	m_LodsDistance = r3d.m_LodsDistance;
	m_CurrentLOD = -1;
	m_IsOpaque = r3d.m_IsOpaque;
	m_DrawShadows = r3d.m_DrawShadows;
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
	Renderer& renderer = Renderer::GetInstance();

	BaseMaterial* material = m_Material->m_BaseMaterial;
	Shader* shader = material->m_Shader->GetPass() == Shader::Pass::AMBIENT ? material->m_Shader : Shader::Get("Default3D");
	shader->Bind();

	for (const auto& [name, value] : material->m_FloatUniformsByName)
	{
		shader->SetUniform1f(name, value);
	}

	for (const auto& [name, value] : material->m_IntUniformsByName)
	{
		shader->SetUniform1i(name, value);
	}

	for (const auto& [name, value] : material->m_Vec2UniformsByName)
	{
		shader->SetUniform2fv(name, value);
	}

	for (const auto& [name, value] : material->m_Vec3UniformsByName)
	{
		shader->SetUniform3fv(name, value);
	}

	for (const auto& [name, value] : material->m_Vec4UniformsByName)
	{
		shader->SetUniform4fv(name, value);
	}

	shader->SetUniformMat4f("u_Transform", m_Owner->m_Transform.GetTransform());
	shader->SetUniformMat3f("u_InverseTransform", glm::transpose(m_Owner->m_Transform.GetInverseTransform()));

	int baseColorIndex = renderer.BindTexture(m_Material->m_BaseColor->GetRendererID());
	int normalColorIndex = renderer.BindTexture(m_Material->m_NormalMap->GetRendererID());
	int metallicColorIndex = renderer.BindTexture(m_Material->m_MetallicMap->GetRendererID());
	int roughnessColorIndex = renderer.BindTexture(m_Material->m_RoughnessMap->GetRendererID());
	int aoColorIndex = renderer.BindTexture(m_Material->m_AoMap->GetRendererID());

	shader->SetUniform1i("u_Shadows.isEnabled", environment.m_ShadowsSettings.m_IsEnabled);

	shader->SetUniform1i("u_MaterialIndex", m_Material->m_UniformIndex);
	shader->SetUniform1i("u_BaseColor", baseColorIndex);
	shader->SetUniform1i("u_NormalColor", normalColorIndex);
	shader->SetUniform1i("u_MetallicColor", metallicColorIndex);
	shader->SetUniform1i("u_RoughnessColor", roughnessColorIndex);
	shader->SetUniform1i("u_AoColor", aoColorIndex);

	std::vector<int> csm(renderer.m_FrameBufferCSM.size());
	for (size_t i = 0; i < csm.size(); i++)
	{
		csm[i] = renderer.BindTexture(renderer.m_FrameBufferCSM[i]->m_Textures[0]);
	}
	shader->SetUniform1iv("u_DirectionalLight.CSM", &csm[0], csm.size());

	for (size_t i = 0; i < renderer.m_PointLights.m_ShadowCubeMaps.size(); i++)
	{
		renderer.BindTexture(renderer.m_PointLights.m_ShadowCubeMaps[i], GL_TEXTURE_CUBE_MAP);
	}

	for (size_t i = 0; i < renderer.m_SpotLights.m_ShadowMaps.size(); i++)
	{
		renderer.BindTexture(renderer.m_SpotLights.m_ShadowMaps[i]);
		shader->SetUniformMat4f(std::string("u_SpotLightShadowMapProjections[" + std::to_string(i) + "]").c_str(), renderer.m_SpotLights.m_ShadowMapsProjections[i]);
	}

	for (const auto& [name, value] : material->m_TextureUniformsByName)
	{
		shader->SetUniform1i(name, renderer.BindTexture(value->GetRendererID()));
	}

	if (GetOwner()->GetScene()->m_DirectionalLights.empty())
	{
		shader->SetUniform1f("u_DirectionalLight.intensity", 0.0f);
	}
	else
	{
		DirectionalLight* directionalLight = GetOwner()->GetScene()->m_DirectionalLights[0];

		shader->SetUniform3fv("u_DirectionalLight.direction", directionalLight->GetOwner()->m_Transform.GetRotationMat4() * glm::vec4(0, 0, -1, 1));
		shader->SetUniform3fv("u_DirectionalLight.color", directionalLight->GetColor());
		shader->SetUniform1f("u_DirectionalLight.intensity", directionalLight->GetIntensity());
		shader->SetUniform1i("u_DirectionalLight.isEnabled", renderer.m_LightSpaceMatrices.empty() ? false : true);
		shader->SetUniformMat4f("u_DirectionalLight.view", environment.GetMainCamera()->GetViewMat4());
		shader->SetUniformfv("u_DirectionalLight.cascadesDistance", directionalLight->GetCascadesDistance());
		shader->SetUniform1i("u_DirectionalLight.pcf", directionalLight->GetPcf());
		shader->SetUniform1f("u_DirectionalLight.fog", directionalLight->GetFog());
		shader->SetUniformfv("u_DirectionalLight.bias", directionalLight->GetBias());
		shader->SetUniform1f("u_DirectionalLight.farPlane", environment.GetMainCamera()->GetZFar() * directionalLight->GetZFarScale());
		shader->SetUniform1i("u_DirectionalLight.isVisualized", environment.m_ShadowsSettings.m_IsVisualized);
		shader->SetUniform1i("u_DirectionalLight.cascadesCount", (int)directionalLight->GetCascadesDistance().size());
		shader->SetUniform1f("u_DirectionalLight.texels", (int)directionalLight->GetTexels());
		shader->SetUniformMat4fv("u_DirectionalLight.lightSpaceMatricies", renderer.m_LightSpaceMatrices);
	}

	shader->SetUniform1i("u_PointLightsSize", renderer.m_PointLights.m_Size);
	shader->SetUniform1i("u_SpotLightsSize", renderer.m_SpotLights.m_Size);
	shader->SetUniform1iv("u_PointLightsShadowMap", &renderer.m_PointLights.m_ShadowSamplers[0], renderer.m_PointLights.m_MaxShadowsSize);
	shader->SetUniform1iv("u_SpotLightsShadowMap", &renderer.m_SpotLights.m_ShadowSamplers[0], renderer.m_SpotLights.m_MaxShadowsSize);

	m_Mesh->m_Va.Bind();
	m_Mesh->m_Ib.Bind();
	
	Editor::GetInstance().m_Stats.m_Triangles += m_Mesh->m_Ib.GetCount() / 3;
	Editor::GetInstance().m_Stats.m_DrawCalls++;

	if (Editor::GetInstance().m_PolygonMode)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	if (m_Mesh->m_CullFace)
	{
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	}
	else
	{
		glDisable(GL_CULL_FACE);
	}

	glDrawElements(GL_TRIANGLES, m_Mesh->m_Ib.GetCount(), GL_UNSIGNED_INT, nullptr);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_CULL_FACE);

	shader->UnBind();

	m_Material->m_BaseMaterial->UnBindTextures();

	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, TextureManager::GetInstance().White()->GetRendererID());
}

IComponent* Renderer3D::Create(GameObject* owner)
{
	Renderer3D* r3d = MemoryManager::GetInstance().Allocate<Renderer3D>();
	r3d->m_Owner = owner;
	r3d->m_Type = GetTypeName<Renderer3D>();
	r3d->SetMaterial(MaterialManager::GetInstance().Load("Source/Materials/Material.mat"));
	owner->GetScene()->m_Renderers3D.push_back(r3d);

	return r3d;
}

void Renderer3D::Delete()
{
	SetMaterial(MaterialManager::GetInstance().Load("Source/Materials/Material.mat"));

	Utils::Erase<Renderer3D*>(GetOwner()->GetScene()->m_Renderers3D, this);
	MemoryManager::GetInstance().FreeMemory<Renderer3D>(static_cast<IAllocator*>(this));
}

void Renderer3D::SortLods(const glm::vec3& cameraPosition)
{
	std::vector<float> lodsDistance = m_LodsDistance;
	glm::vec3 position = GetOwner()->m_Transform.GetPosition();
	float distance2 = glm::distance2(cameraPosition, position);

	if (distance2 < lodsDistance[0] * lodsDistance[0])
	{
		SetCurrentLod(0);
	}
	else if (distance2 < lodsDistance[1] * lodsDistance[1])
	{
		SetCurrentLod(1);
	}
	else
	{
		SetCurrentLod(2);
	}
}

void Renderer3D::SetMesh(Mesh* mesh, size_t lod)
{
	if (lod >= m_Lods.size())
	{
		lod = 0;
	}

	m_Mesh = mesh;
	m_Lods[lod] = m_Mesh;
}

void Renderer3D::SetMaterial(Material* material)
{
	m_Material = material;
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
