#include "Instancing.h"

#include "Environment.h"
#include "Editor.h"
#include "Renderer.h"
#include "Viewport.h"
#include "../OpenGL/FrameBuffer.h"
#include "../OpenGL/Material.h"
#include "../OpenGL/BaseMaterial.h"

using namespace Pengine;

Instancing::Instancing()
{
	Logger::Success("Instancing has been initialized!");
}

Instancing& Instancing::GetInstance()
{
	static Instancing instancing;
	return instancing;
}

/*
 * Deprecated. 
void Instancing::Render(const std::unordered_map<Mesh*, std::vector<Renderer3D*>>& instancedObjects,
	std::unordered_map<Mesh*, DynamicBuffer>& bufferByMesh)
{
	if (instancedObjects.empty())
	{
		return;
	}

	Shader* shader = Shader::Get("Instancing3D");
	shader->Bind();

	if (Editor::GetInstance().m_PolygonMode)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Environment& environment = Environment::GetInstance();

	shader->SetUniform3fv("u_CameraPosition", environment.GetMainCamera()->m_Transform.GetPosition());
	shader->SetUniformMat4f("u_ViewProjection", environment.GetMainCamera()->GetViewProjectionMat4());
	shader->SetUniform2fv("u_ViewportSize", (glm::vec2)Viewport::GetInstance().GetSize());
	shader->SetUniformMat4f("u_View", environment.GetMainCamera()->GetViewMat4());
	shader->SetUniformfv("u_CascadesDistance", environment.m_ShadowsSettings.m_CascadesDistance);
	shader->SetUniform1i("u_Pcf", environment.m_ShadowsSettings.m_Pcf);
	shader->SetUniform1f("u_Fog", environment.m_ShadowsSettings.m_Fog);
	shader->SetUniform1f("u_Bias", environment.m_ShadowsSettings.m_Bias);
	shader->SetUniform1f("u_FarPlane", environment.GetMainCamera()->GetZFar() * environment.m_ShadowsSettings.m_ZFarScale);
	shader->SetUniform1i("u_ShadowsEnabled", environment.m_ShadowsSettings.m_IsEnabled);
	shader->SetUniform1i("u_ShadowsVisualized", environment.m_ShadowsSettings.m_IsVisualized);
	shader->SetUniform1i("u_CascadesCount", (int)environment.m_ShadowsSettings.m_CascadesDistance.size());
	shader->SetUniform1f("u_Texels", (int)environment.m_ShadowsSettings.m_Texels);
	shader->SetUniformMat4fv("u_LightSpaceMatricies", Renderer::GetInstance().m_LightSpaceMatrices);

	Scene* scene = instancedObjects.begin()->second[0]->GetOwner()->GetScene();

	if (scene->m_DirectionalLights.empty())
	{
		shader->SetUniform1f("u_DirectionalLight.intensity", 0.0f);
	}
	else
	{
		DirectionalLight* directionalLight = scene->m_DirectionalLights[0];

		shader->SetUniform3fv("u_DirectionalLight.direction", directionalLight->GetDirection());
		shader->SetUniform3fv("u_DirectionalLight.color", directionalLight->m_Color);
		shader->SetUniform1f("u_DirectionalLight.intensity", directionalLight->m_Intensity);
	}

	int pointLightsSize = scene->m_PointLights.size();
	shader->SetUniform1i("pointLightsSize", pointLightsSize);
	char uniformBuffer[64];
	for (int i = 0; i < pointLightsSize; i++)
	{
		PointLight* pointLight = scene->m_PointLights[i];
		sprintf(uniformBuffer, "pointLights[%i].position", i);
		shader->SetUniform3fv(uniformBuffer, pointLight->GetOwner()->m_Transform.GetPosition());
		sprintf(uniformBuffer, "pointLights[%i].color", i);
		shader->SetUniform3fv(uniformBuffer, pointLight->m_Color);
		sprintf(uniformBuffer, "pointLights[%i].constant", i);
		shader->SetUniform1f(uniformBuffer, pointLight->m_Constant);
		sprintf(uniformBuffer, "pointLights[%i]._linear", i);
		shader->SetUniform1f(uniformBuffer, pointLight->m_Linear);
		sprintf(uniformBuffer, "pointLights[%i].quadratic", i);
		shader->SetUniform1f(uniformBuffer, pointLight->m_Quadratic);
	}

	int spotLightsSize = scene->m_SpotLights.size();
	shader->SetUniform1i("spotLightsSize", spotLightsSize);
	for (int i = 0; i < spotLightsSize; i++)
	{
		SpotLight* spotLight = scene->m_SpotLights[i];
		sprintf(uniformBuffer, "spotLights[%i].position", i);
		shader->SetUniform3fv(uniformBuffer, spotLight->GetOwner()->m_Transform.GetPosition());
		sprintf(uniformBuffer, "spotLights[%i].direction", i);
		shader->SetUniform3fv(uniformBuffer, spotLight->GetDirection());
		sprintf(uniformBuffer, "spotLights[%i].color", i);
		shader->SetUniform3fv(uniformBuffer, spotLight->m_Color);
		sprintf(uniformBuffer, "spotLights[%i].constant", i);
		shader->SetUniform1f(uniformBuffer, spotLight->m_Constant);
		sprintf(uniformBuffer, "spotLights[%i]._linear", i);
		shader->SetUniform1f(uniformBuffer, spotLight->m_Linear);
		sprintf(uniformBuffer, "spotLights[%i].quadratic", i);
		shader->SetUniform1f(uniformBuffer, spotLight->m_Quadratic);
		sprintf(uniformBuffer, "spotLights[%i].innerCutOff", i);
		shader->SetUniform1f(uniformBuffer, (spotLight->m_CosInnerCutOff));
		sprintf(uniformBuffer, "spotLights[%i].outerCutOff", i);
		shader->SetUniform1f(uniformBuffer, (spotLight->m_CosOuterCutOff));
	}

	for (auto instancedObject = instancedObjects.begin(); instancedObject != instancedObjects.end(); instancedObject++)
	{
		auto buffer = bufferByMesh.find(instancedObject->first);
		if (buffer == bufferByMesh.end())
		{
			continue;
		}

		size_t objectsSize = instancedObject->second.size();
		if (objectsSize == 0)
		{
			continue;
		}

		Mesh& mesh = *buffer->first;
		DynamicBuffer& dynamicBuffer = buffer->second;

		if (!dynamicBuffer.m_VertAttrib)
		{
			continue;
		}

		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, TextureManager::GetInstance().White()->GetRendererID());
		dynamicBuffer.m_TextureSlots[0] = TextureManager::GetInstance().White()->GetRendererID();

		Editor::GetInstance().m_Stats.m_Triangles += (mesh.m_Ib.GetCount() * objectsSize) / 3;
		Editor::GetInstance().m_Stats.m_DrawCalls++;

		mesh.m_Va.Bind();
		mesh.m_Ib.Bind();
		mesh.m_Vb.Bind();

		int samplers[32];
		for (size_t i = m_TextureOffset; i < dynamicBuffer.m_TextureSlotsIndex; i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, dynamicBuffer.m_TextureSlots[i]);
		}
		for (size_t i = 0; i < maxTextureSlots; i++)
		{
			samplers[i] = i;
		}
		shader->SetUniform1iv("u_Texture", samplers);

		std::vector<int> csm(Renderer::GetInstance().m_FrameBufferCSM.size());
		for (size_t i = 0; i < csm.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i + 1);
			glBindTexture(GL_TEXTURE_2D, Renderer::GetInstance().m_FrameBufferCSM[i]->m_Textures[0]);
			csm[i] = i + 1;
		}

		shader->SetUniform1iv("u_CSM", &csm[0], csm.size());

		if (instancedObject->second[0]->m_BackFaceCulling)
		{
			glEnable(GL_CULL_FACE);
		}

		glDrawElementsInstanced(GL_TRIANGLES, mesh.m_Ib.GetCount(), GL_UNSIGNED_INT, 0, instancedObject->second.size());

		dynamicBuffer.m_TextureSlotsIndex = m_TextureOffset;
		for (size_t i = dynamicBuffer.m_TextureSlotsIndex; i < 32; i++)
		{
			dynamicBuffer.m_TextureSlots[i] = 0;
		}
	}

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_CULL_FACE);

	shader->UnBind();
}
*/

void Instancing::RenderGBuffer(const std::unordered_map<BaseMaterial*, Scene::Renderable>& instancedObjects)
{
	if (instancedObjects.empty())
	{
		return;
	}

	for (const auto& [material, renderable] : instancedObjects)
	{
		Shader* shader = material->m_Shader->GetPass() == Shader::Pass::DEFERRED ? material->m_Shader : Shader::Get("InstancingGBuffer");;
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

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		if (Editor::GetInstance().m_PolygonMode)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}

		for (const auto& [mesh, r3ds] : renderable)
		{
			mesh->m_DynamicBufferOpaqueByMaterial[material].Initialize();
			Mesh::DynamicBuffer& buffer = mesh->m_DynamicBufferOpaqueByMaterial[material];

			for (const auto& [name, value] : material->m_TextureUniformsByName)
			{
				shader->SetUniform1i(name, material->BindTexture(value->GetRendererID()));
			}

			for (const auto& r3d : r3ds)
			{
				r3d->m_Material->m_BaseColorIndex = material->BindTexture(r3d->m_Material->m_BaseColor->GetRendererID());
				r3d->m_Material->m_NormalMapIndex = material->BindTexture(r3d->m_Material->m_NormalMap->GetRendererID());
				r3d->m_Material->m_MetallicIndex = material->BindTexture(r3d->m_Material->m_MetallicMap->GetRendererID());
				r3d->m_Material->m_RoughnessIndex = material->BindTexture(r3d->m_Material->m_RoughnessMap->GetRendererID());
				r3d->m_Material->m_AoIndex = material->BindTexture(r3d->m_Material->m_AoMap->GetRendererID());
			}

			std::vector<int> samplers = material->BindTextures();
			shader->SetUniform1iv("u_Texture", &samplers[0], samplers.size());

			size_t objectsSize = r3ds.size();
			if (objectsSize == 0)
			{
				continue;
			}

			if (buffer.m_VertexAttributes.empty())
			{
				continue;
			}

			mesh->m_Va.Bind();

			buffer.m_VboDynamic.Bind();
			if (buffer.m_AllocateNewBuffer)
			{
				glBufferData(GL_ARRAY_BUFFER, buffer.m_VertexAttributes.size() * sizeof(float), &buffer.m_VertexAttributes[0], GL_DYNAMIC_DRAW);
			}
			else
			{
				glBufferSubData(GL_ARRAY_BUFFER, 0, buffer.m_VertexAttributes.size() * sizeof(float), &buffer.m_VertexAttributes[0]);
			}

			mesh->m_Va.AddBuffer(buffer.m_VboDynamic, buffer.m_LayoutDynamic, Mesh::GetStaticAttributeVertexOffset(), 1);

			Editor::GetInstance().m_Stats.m_Triangles += (mesh->m_Ib.GetCount() * objectsSize) / 3;
			Editor::GetInstance().m_Stats.m_DrawCalls++;

			if (mesh->m_CullFace)
			{
				glEnable(GL_CULL_FACE);
				glEnable(GL_BACK);
			}
			else
			{
				glDisable(GL_CULL_FACE);
			}

			mesh->m_Va.Bind();
			mesh->m_Ib.Bind();
			mesh->m_Vb.Bind();
			buffer.m_VboDynamic.Bind();

			glDrawElementsInstanced(GL_TRIANGLES, mesh->m_Ib.GetCount(), GL_UNSIGNED_INT, 0, r3ds.size());

			material->UnBindTextures();
		}

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDisable(GL_CULL_FACE);
		
		shader->UnBind();
	}
}

void Instancing::RenderShadowsObjects(const std::unordered_map<Mesh*, std::vector<Renderer3D*>>& instancedObjects)
{
	if (instancedObjects.empty())
	{
		return;
	}

	for (const auto& [mesh, r3ds] : instancedObjects)
	{
		Mesh::DynamicBufferShadows& buffer = mesh->m_DynamicBufferShadows;

		size_t objectsSize = r3ds.size();
		if (objectsSize == 0)
		{
			continue;
		}

		if (buffer.m_VertexAttributes.empty())
		{
			continue;
		}

		Editor::GetInstance().m_Stats.m_Triangles += (mesh->m_Ib.GetCount() * objectsSize) / 3;
		Editor::GetInstance().m_Stats.m_DrawCalls++;

		mesh->m_Va.Bind();
		mesh->m_Ib.Bind();
		mesh->m_Vb.Bind();
		
		if (mesh->m_ShadowCullFace == GL_FRONT_AND_BACK)
		{
			glDisable(GL_CULL_FACE);
			glDrawElementsInstanced(GL_TRIANGLES, mesh->m_Ib.GetCount(), GL_UNSIGNED_INT, 0, r3ds.size());
		}
		else
		{
			glEnable(GL_CULL_FACE);
			glCullFace(mesh->m_ShadowCullFace);
			glDrawElementsInstanced(GL_TRIANGLES, mesh->m_Ib.GetCount(), GL_UNSIGNED_INT, 0, r3ds.size());
			glCullFace(GL_BACK);
		}
	}
}

void Instancing::PrepareVertexAtrrib(const std::unordered_map<BaseMaterial*, Scene::Renderable>& instancedObjects)
{
	for (const auto& [material, renderable] : instancedObjects)
	{
		for (const auto& [mesh, r3ds] : renderable)
		{
			Mesh::DynamicBuffer& buffer = mesh->m_DynamicBufferOpaqueByMaterial[material];
			buffer.Initialize();

			size_t objectsSize = r3ds.size();
			if (objectsSize == 0)
			{
				continue;
			}

			buffer.m_AllocateNewBuffer = false;

			if (buffer.m_PrevObjectSize < objectsSize || buffer.m_VertexAttributes.empty())
			{
				buffer.m_VertexAttributes.resize(objectsSize * m_SizeOfAttribs);
				buffer.m_AllocateNewBuffer = true;
			}

			buffer.m_PrevObjectSize = objectsSize;

			for (size_t i = 0; i < r3ds.size(); i++)
			{
				Renderer3D* r3d = r3ds[i];

				GameObject& owner = *(r3d->GetOwner());

				float* startPtr = &buffer.m_VertexAttributes[i * m_SizeOfAttribs];

				const glm::mat4 transformMat4 = owner.m_Transform.GetTransform();
				const float* transform = glm::value_ptr(transformMat4);
				memcpy(&startPtr[0], transform, 64);

				const glm::mat3 inverseTransformMat3 = glm::transpose(owner.m_Transform.GetInverseTransform());
				const float* inverseTransform = glm::value_ptr(inverseTransformMat3);
				memcpy(&startPtr[16], inverseTransform, 36);

				startPtr[25] = (float)r3d->m_Material->m_UniformIndex;
			}

			continue;
		}
	}
}

void Instancing::PrepareShadowsVertexAtrrib(const std::unordered_map<Mesh*, std::vector<Renderer3D*>>& instancedObjects)
{
	for (const auto& [mesh, r3ds] : instancedObjects)
	{
		Mesh::DynamicBufferShadows& shadowsBuffer = mesh->m_DynamicBufferShadows;
		size_t rejectedShadows = 0;

		size_t objectsSize = r3ds.size();
		if (objectsSize == 0)
		{
			continue;
		}

		shadowsBuffer.m_AllocateNewBuffer = false;

		if (shadowsBuffer.m_PrevObjectSize < objectsSize || shadowsBuffer.m_VertexAttributes.empty())
		{
			shadowsBuffer.m_VertexAttributes.resize(objectsSize * 16);
			shadowsBuffer.m_AllocateNewBuffer = true;
		}

		shadowsBuffer.m_PrevObjectSize = objectsSize;

		for (size_t i = 0; i < r3ds.size(); i++)
		{
			Renderer3D* r3d = r3ds[i];

			rejectedShadows += !r3d->m_DrawShadows;

			if (r3d->m_DrawShadows)
			{
				GameObject& owner = *(r3d->GetOwner());

				const glm::mat4 transformMat4 = owner.m_Transform.GetTransform();
				const float* transform = glm::value_ptr(transformMat4);

				float* startPtr = &shadowsBuffer.m_VertexAttributes[(i - rejectedShadows) * 16];
				memcpy(&startPtr[0], transform, 64);
			}
		}
	}
}


void Instancing::BindShadowsBuffers(const std::unordered_map<Mesh*, std::vector<Renderer3D*>>& instancedObjects)
{
	if (instancedObjects.empty())
	{
		return;
	}

	for (const auto& [mesh, r3ds] : instancedObjects)
	{
		Mesh::DynamicBufferShadows& buffer = mesh->m_DynamicBufferShadows;

		size_t objectsSize = r3ds.size();
		if (objectsSize == 0)
		{
			continue;
		}

		if (buffer.m_VertexAttributes.empty())
		{
			continue;
		}

		mesh->m_Va.Bind();

		buffer.m_VboDynamic.Bind();
		if (buffer.m_AllocateNewBuffer)
		{
			glBufferData(GL_ARRAY_BUFFER, buffer.m_VertexAttributes.size() * sizeof(float), &buffer.m_VertexAttributes[0], GL_DYNAMIC_DRAW);
		}
		else
		{
			glBufferSubData(GL_ARRAY_BUFFER, 0, buffer.m_VertexAttributes.size() * sizeof(float), &buffer.m_VertexAttributes[0]);
		}

		mesh->m_Va.AddBuffer(buffer.m_VboDynamic, buffer.m_LayoutDynamic, Mesh::GetStaticAttributeVertexOffset(), 1);
	}
}