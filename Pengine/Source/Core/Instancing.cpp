#include "Instancing.h"

#include "Environment.h"
#include "Editor.h"
#include "Renderer.h"
#include "Viewport.h"
#include "../OpenGL/FrameBuffer.h"

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

void Instancing::Create(Mesh* mesh)
{
	std::pair<Mesh*, DynamicBuffer> bufferByMesh = std::make_pair(mesh, DynamicBuffer());
	bufferByMesh.second.m_VboDynamic.Initialize(nullptr, 1, false);
	bufferByMesh.second.m_LayoutDynamic.Push<float>(3);
	bufferByMesh.second.m_LayoutDynamic.Push<float>(3);
	bufferByMesh.second.m_LayoutDynamic.Push<float>(3);
	bufferByMesh.second.m_LayoutDynamic.Push<float>(4);
	bufferByMesh.second.m_LayoutDynamic.Push<float>(4);
	bufferByMesh.second.m_LayoutDynamic.Push<float>(4);
	bufferByMesh.second.m_LayoutDynamic.Push<float>(4);
	bufferByMesh.second.m_LayoutDynamic.Push<float>(4);
	bufferByMesh.second.m_LayoutDynamic.Push<float>(3);
	bufferByMesh.second.m_LayoutDynamic.Push<float>(3);
	bufferByMesh.second.m_LayoutDynamic.Push<float>(3);

	m_OpaqueBuffersByMesh.insert(bufferByMesh);

	std::pair<Mesh*, DynamicBuffer> shadowBufferByMesh = std::make_pair(mesh, DynamicBuffer());
	shadowBufferByMesh.second.m_VboDynamic.Initialize(nullptr, 1, false);
	shadowBufferByMesh.second.m_LayoutDynamic.Push<float>(4);
	shadowBufferByMesh.second.m_LayoutDynamic.Push<float>(4);
	shadowBufferByMesh.second.m_LayoutDynamic.Push<float>(4);
	shadowBufferByMesh.second.m_LayoutDynamic.Push<float>(4);

	m_ShadowsBuffersByMesh.insert(shadowBufferByMesh);
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

void Instancing::RenderGBuffer(const std::unordered_map<Mesh*, std::vector<Renderer3D*>>& instancedObjects,
	std::unordered_map<Mesh*, DynamicBuffer>& bufferByMesh)
{
	if (instancedObjects.empty())
	{
		return;
	}

	Shader* shader = Shader::Get("InstancingGBuffer");
	shader->Bind();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (Editor::GetInstance().m_PolygonMode)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	shader->SetUniformMat4f("u_ViewProjection", Environment::GetInstance().GetMainCamera()->GetViewProjectionMat4());

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

		Editor::GetInstance().m_Stats.m_Triangles += (mesh.m_Ib.GetCount() * objectsSize) / 3;
		Editor::GetInstance().m_Stats.m_DrawCalls++;

		mesh.m_Va.Bind();
		mesh.m_Ib.Bind();
		mesh.m_Vb.Bind();
		
		int samplers[32];
		for (size_t i = 0; i < dynamicBuffer.m_TextureSlotsIndex; i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, dynamicBuffer.m_TextureSlots[i]);
		}
		for (size_t i = 0; i < maxTextureSlots; i++)
		{
			samplers[i] = i;
		}
		shader->SetUniform1iv("u_Texture", samplers);

		if (instancedObject->first->m_CullFace)
		{
			glEnable(GL_CULL_FACE);
		}
		else
		{
			glDisable(GL_CULL_FACE);
		}

		mesh.m_Va.Bind();
		mesh.m_Ib.Bind();
		mesh.m_Vb.Bind();
		dynamicBuffer.m_VboDynamic.Bind();

		glDrawElementsInstanced(GL_TRIANGLES, mesh.m_Ib.GetCount(), GL_UNSIGNED_INT, 0, instancedObject->second.size());

		dynamicBuffer.m_TextureSlotsIndex = 0;
		for (size_t i = 0; i < 32; i++)
		{
			dynamicBuffer.m_TextureSlots[i] = 0;
		}
	}

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_CULL_FACE);

	shader->UnBind();
}

void Instancing::RenderShadowsObjects(const std::unordered_map<Mesh*, std::vector<Renderer3D*>>& instancedObjects,
	std::unordered_map<Mesh*, DynamicBuffer>& bufferByMesh)
{
	if (instancedObjects.empty())
	{
		return;
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

		Editor::GetInstance().m_Stats.m_Triangles += (mesh.m_Ib.GetCount() * objectsSize) / 3;
		Editor::GetInstance().m_Stats.m_DrawCalls++;

		mesh.m_Va.Bind();
		mesh.m_Ib.Bind();
		mesh.m_Vb.Bind();

		glDrawElementsInstanced(GL_TRIANGLES, mesh.m_Ib.GetCount(), GL_UNSIGNED_INT, 0, instancedObject->second.size());
	}
}

void Instancing::PrepareVertexAtrrib(const std::unordered_map<Mesh*, std::vector<Renderer3D*>>& instancedObjects)
{
	for (auto instancedObject = instancedObjects.begin(); instancedObject != instancedObjects.end(); instancedObject++)
	{
		auto buffer = m_OpaqueBuffersByMesh.find(instancedObject->first);
		if (buffer == m_OpaqueBuffersByMesh.end())
		{
			continue;
		}

		auto shadowsBuffer = m_ShadowsBuffersByMesh.find(instancedObject->first);
		size_t rejectedShadows = 0;

		size_t objectsSize = instancedObject->second.size();
		if (objectsSize == 0)
		{
			continue;
		}

		Mesh& mesh = *buffer->first;
		DynamicBuffer& dynamicBuffer = buffer->second;
		DynamicBuffer& dynamicShadowsBuffer = shadowsBuffer->second;

		dynamicBuffer.m_AllocateNewBuffer = false;
		dynamicShadowsBuffer.m_AllocateNewBuffer = false;

		if (dynamicBuffer.m_PrevObjectSize < objectsSize || dynamicBuffer.m_VertAttrib == nullptr)
		{
			delete[] dynamicBuffer.m_VertAttrib;
			dynamicBuffer.m_Size = objectsSize * m_SizeOfAttribs;
			dynamicBuffer.m_VertAttrib = new float[dynamicBuffer.m_Size];
			dynamicBuffer.m_AllocateNewBuffer = true;
		}

		if (dynamicShadowsBuffer.m_PrevObjectSize < objectsSize || dynamicShadowsBuffer.m_VertAttrib == nullptr)
		{
			delete[] dynamicShadowsBuffer.m_VertAttrib;
			dynamicShadowsBuffer.m_Size = objectsSize * 16;
			dynamicShadowsBuffer.m_VertAttrib = new float[dynamicShadowsBuffer.m_Size];
			dynamicShadowsBuffer.m_AllocateNewBuffer = true;
		}

		dynamicBuffer.m_PrevObjectSize = objectsSize;
		dynamicShadowsBuffer.m_PrevObjectSize = objectsSize;

		// TODO: Chacing texture index in Renderer3D by calling GetTextureIndex and pass it here!
		for (size_t i = 0; i < objectsSize; i++)
		{
			Renderer3D& r3d = *(instancedObject->second[i]);
			Material& material = *r3d.m_Material;
				
			rejectedShadows += !r3d.m_DrawShadows;

			auto bindTexture = [&dynamicBuffer](Texture* texture)
			{
				int textureIndex = 0;
				
				auto instancedObject = std::find(dynamicBuffer.m_TextureSlots.begin(), dynamicBuffer.m_TextureSlots.end(), texture->GetRendererID());
				if (instancedObject != dynamicBuffer.m_TextureSlots.end())
				{
					textureIndex = (instancedObject - dynamicBuffer.m_TextureSlots.begin());
				}
				else
				{
					if (dynamicBuffer.m_TextureSlotsIndex >= 32)
					{
						Logger::Warning("limit of texture slots, texture will be set to white!");
					}
					else
					{
						textureIndex = dynamicBuffer.m_TextureSlotsIndex;
						dynamicBuffer.m_TextureSlots[dynamicBuffer.m_TextureSlotsIndex] = texture->GetRendererID();
						dynamicBuffer.m_TextureSlotsIndex++;
					}
				}

				return textureIndex;
			};
			
			int baseColorIndex = bindTexture(r3d.m_Material->m_BaseColor);
			int NormalMapIndex = bindTexture(r3d.m_Material->m_NormalMap);

			GameObject& owner = *(r3d.GetOwner());

			const glm::vec3 ambient = material.m_Ambient * material.m_Scale;
			const glm::vec3 diffuse = material.m_Diffuse * material.m_Scale;
			const glm::vec3 specular = material.m_Specular * material.m_Scale;

			float* startPtr = &dynamicBuffer.m_VertAttrib[i * m_SizeOfAttribs];
			memcpy(&startPtr[0], &ambient[0], 12);
			memcpy(&startPtr[3], &diffuse[0], 12);
			memcpy(&startPtr[6], &specular[0], 12);

			startPtr[9] = (float)baseColorIndex;
			startPtr[10] = (float)NormalMapIndex;
			startPtr[11] = material.m_Shininess;
			startPtr[12] = (float)material.m_UseNormalMap;

			const glm::mat4 transformMat4 = owner.m_Transform.GetTransform();
			const float* transform = glm::value_ptr(transformMat4);
			memcpy(&startPtr[13], transform, 64);

			const glm::mat3 inverseTransformMat3 = glm::transpose(owner.m_Transform.GetInverseTransform());
			const float* inverseTransform = glm::value_ptr(inverseTransformMat3);
			memcpy(&startPtr[29], inverseTransform, 36);

			if (r3d.m_DrawShadows)
			{
				float* startPtr = &dynamicShadowsBuffer.m_VertAttrib[(i - rejectedShadows) * 16];
				memcpy(&startPtr[0], transform, 64);
			}
		}

		continue;
	}
}

void Instancing::BindBuffers(const std::unordered_map<Mesh*, std::vector<Renderer3D*>>& instancedObjects)
{
	if (instancedObjects.empty())
	{
		return;
	}

	for (auto instancedObject = instancedObjects.begin(); instancedObject != instancedObjects.end(); instancedObject++)
	{
		auto buffer = m_OpaqueBuffersByMesh.find(instancedObject->first);
		if (buffer == m_OpaqueBuffersByMesh.end())
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

		mesh.m_Va.Bind();

		dynamicBuffer.m_VboDynamic.Bind();
		if (dynamicBuffer.m_AllocateNewBuffer)
		{
			glBufferData(GL_ARRAY_BUFFER, dynamicBuffer.m_Size * sizeof(float), dynamicBuffer.m_VertAttrib, GL_DYNAMIC_DRAW);
		}
		else
		{
			glBufferSubData(GL_ARRAY_BUFFER, 0, dynamicBuffer.m_Size * sizeof(float), dynamicBuffer.m_VertAttrib);
		}

		mesh.m_Va.AddBuffer(dynamicBuffer.m_VboDynamic, dynamicBuffer.m_LayoutDynamic, Mesh::GetStaticAttributeVertexOffset(), 1);
	}
}

void Instancing::BindShadowsBuffers(const std::unordered_map<Mesh*, std::vector<Renderer3D*>>& instancedObjects)
{
	if (instancedObjects.empty())
	{
		return;
	}

	for (auto instancedObject = instancedObjects.begin(); instancedObject != instancedObjects.end(); instancedObject++)
	{
		auto buffer = m_ShadowsBuffersByMesh.find(instancedObject->first);
		if (buffer == m_ShadowsBuffersByMesh.end())
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

		mesh.m_Va.Bind();

		dynamicBuffer.m_VboDynamic.Bind();
		if (dynamicBuffer.m_AllocateNewBuffer)
		{
			glBufferData(GL_ARRAY_BUFFER, dynamicBuffer.m_Size * sizeof(float), dynamicBuffer.m_VertAttrib, GL_DYNAMIC_DRAW);
		}
		else
		{
			glBufferSubData(GL_ARRAY_BUFFER, 0, dynamicBuffer.m_Size * sizeof(float), dynamicBuffer.m_VertAttrib);
		}

		mesh.m_Va.AddBuffer(dynamicBuffer.m_VboDynamic, dynamicBuffer.m_LayoutDynamic, Mesh::GetStaticAttributeVertexOffset(), 1);
	}
}

void Instancing::ShutDown()
{
	for (auto buffer : m_OpaqueBuffersByMesh)
	{
		delete buffer.second.m_VertAttrib;
	}

	for (auto buffer : m_ShadowsBuffersByMesh)
	{
		delete buffer.second.m_VertAttrib;
	}
}
