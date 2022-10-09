#include "Instancing.h"

#include "Environment.h"
#include "Editor.h"
#include "Renderer.h"
#include "Viewport.h"
#include "../OpenGL/FrameBuffer.h"

using namespace Pengine;

Instancing::Instancing()
	: m_NThreads(11)
	, m_SyncParams(m_NThreads)
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
	m_BuffersByMesh.insert(std::make_pair(mesh, DynamicBuffer()));
	m_BuffersByMesh.rbegin()->second.m_VboDynamic.Initialize(nullptr, 1, false);
}

void Instancing::Render(const std::map<Mesh*, std::vector<Renderer3D*>>& instancedObjects)
{
	if (instancedObjects.empty())
	{
		return;
	}

	Shader* shader = Shader::Get("Instancing3D");
	shader->Bind();

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
	shader->SetUniform1i("u_IsShadowBlurEnabled", environment.m_ShadowsSettings.m_Blur.m_IsEnabled);
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
		auto buffer = m_BuffersByMesh.find(instancedObject->first);
		if (buffer == m_BuffersByMesh.end())
		{
			continue;
		}

		size_t objectsSize = instancedObject->second.size();
		if (objectsSize == 0)
		{
			continue;
		}

		if (!buffer->second.m_VertAttrib)
		{
			continue;
		}

		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, TextureManager::GetInstance().White()->GetRendererID());
		buffer->second.m_TextureSlots[0] = TextureManager::GetInstance().White()->GetRendererID();

		Editor::GetInstance().m_Stats.m_Indices += buffer->first->GetIndices().size() * objectsSize;
		Editor::GetInstance().m_Stats.m_Vertices += ((buffer->second.m_Size + buffer->first->GetVertexAttributes().size()) / m_SizeOfAttribs)
			* objectsSize;
		Editor::GetInstance().m_Stats.m_DrawCalls++;

		if (objectsSize > m_SizeOfObjectToThreads - 1)
		{
			m_SyncParams.WaitForAllThreads();
		}

		buffer->first->m_Va.Bind();
		buffer->first->m_Ib.Bind();
		buffer->first->m_Vb.Bind();
		buffer->second.m_VboDynamic.Bind();
		glBufferData(GL_ARRAY_BUFFER, buffer->second.m_Size * sizeof(float), buffer->second.m_VertAttrib, GL_DYNAMIC_DRAW);
		buffer->second.m_LayoutDynamic.Clear();
		buffer->second.m_LayoutDynamic.Push<float>(3);
		buffer->second.m_LayoutDynamic.Push<float>(3);
		buffer->second.m_LayoutDynamic.Push<float>(3);
		buffer->second.m_LayoutDynamic.Push<float>(3);
		buffer->second.m_LayoutDynamic.Push<float>(3);
		buffer->second.m_LayoutDynamic.Push<float>(2);
		buffer->second.m_LayoutDynamic.Push<float>(4);
		buffer->second.m_LayoutDynamic.Push<float>(4);
		buffer->second.m_LayoutDynamic.Push<float>(4);
		buffer->second.m_LayoutDynamic.Push<float>(4);
		buffer->first->m_Va.AddBuffer(buffer->second.m_VboDynamic, buffer->second.m_LayoutDynamic, 4, 1);

		int samplers[32];
		for (unsigned int i = m_TextureOffset; i < buffer->second.m_TextureSlotsIndex; i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, buffer->second.m_TextureSlots[i]);
		}
		for (unsigned int i = 0; i < maxTextureSlots; i++)
		{
			samplers[i] = i;
		}
		shader->SetUniform1iv("u_Texture", samplers);

		if (environment.m_ShadowsSettings.m_Blur.m_IsEnabled)
		{
			glActiveTexture(GL_TEXTURE0 + 1);
			glBindTexture(GL_TEXTURE_2D, Renderer::GetInstance().m_FrameBufferShadowsBlur[0]->m_Textures[0]);
		}
		else
		{
			std::vector<int> csm(Renderer::GetInstance().m_FrameBufferCSM.size());
			for (size_t i = 0; i < csm.size(); i++)
			{
				glActiveTexture(GL_TEXTURE0 + i + 1);
				glBindTexture(GL_TEXTURE_2D, Renderer::GetInstance().m_FrameBufferCSM[i]->m_Textures[0]);
				csm[i] = i + 1;
				shader->SetUniform1iv("u_CSM", &csm[0], csm.size());
			}
		}

		if (instancedObject->second[0]->m_BackFaceCulling)
		{
			glEnable(GL_CULL_FACE);
		}

		if (Editor::GetInstance().m_PolygonMode)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glDrawElementsInstanced(GL_TRIANGLES, buffer->first->m_Ib.GetCount(), GL_UNSIGNED_INT, 0, instancedObject->second.size());

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDisable(GL_CULL_FACE);

		buffer->first->m_Ib.UnBind();
		buffer->first->m_Va.UnBind();

		buffer->second.m_TextureSlotsIndex = m_TextureOffset;
		for (unsigned int i = buffer->second.m_TextureSlotsIndex; i < 32; i++)
		{
			buffer->second.m_TextureSlots[i] = 0;
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, buffer->second.m_TextureSlots[0]);
		}

		glDisable(GL_TEXTURE_2D_ARRAY);
	}

	shader->UnBind();
}

void Instancing::RenderAllObjects(const std::map<Mesh*, std::vector<Renderer3D*>>& instancedObjects)
{
	if (instancedObjects.empty())
	{
		return;
	}

	for (auto instancedObject = instancedObjects.begin(); instancedObject != instancedObjects.end(); instancedObject++)
	{
		auto buffer = m_BuffersByMesh.find(instancedObject->first);
		if (buffer == m_BuffersByMesh.end())
		{
			continue;
		}

		size_t objectsSize = instancedObject->second.size();
		if (objectsSize == 0)
		{
			continue;
		}

		if (!buffer->second.m_VertAttrib)
		{
			continue;
		}

		Editor::GetInstance().m_Stats.m_Indices += buffer->first->GetIndices().size() * objectsSize;
		Editor::GetInstance().m_Stats.m_Vertices += ((buffer->second.m_Size + buffer->first->GetVertexAttributes().size()) / m_SizeOfAttribs)
			* objectsSize;
		Editor::GetInstance().m_Stats.m_DrawCalls++;

		buffer->first->m_Va.Bind();
		buffer->first->m_Ib.Bind();
		buffer->first->m_Vb.Bind();
		buffer->second.m_VboDynamic.Bind();
		glBufferData(GL_ARRAY_BUFFER, buffer->second.m_Size * sizeof(float), buffer->second.m_VertAttrib, GL_DYNAMIC_DRAW);
		buffer->second.m_LayoutDynamic.Clear();
		buffer->second.m_LayoutDynamic.Push<float>(3);
		buffer->second.m_LayoutDynamic.Push<float>(3);
		buffer->second.m_LayoutDynamic.Push<float>(3);
		buffer->second.m_LayoutDynamic.Push<float>(3);
		buffer->second.m_LayoutDynamic.Push<float>(3);
		buffer->second.m_LayoutDynamic.Push<float>(2);
		buffer->second.m_LayoutDynamic.Push<float>(4);
		buffer->second.m_LayoutDynamic.Push<float>(4);
		buffer->second.m_LayoutDynamic.Push<float>(4);
		buffer->second.m_LayoutDynamic.Push<float>(4);
		buffer->first->m_Va.AddBuffer(buffer->second.m_VboDynamic, buffer->second.m_LayoutDynamic, 4, 1);

		glDrawElementsInstanced(GL_TRIANGLES, buffer->first->m_Ib.GetCount(), GL_UNSIGNED_INT, 0, instancedObject->second.size());

		buffer->first->m_Ib.UnBind();
		buffer->first->m_Va.UnBind();
	}
}

void Instancing::PrepareVertexAtrrib(const std::map<Mesh*, std::vector<Renderer3D*>>& instancedObjects)
{
	for (auto instancedObject = instancedObjects.begin(); instancedObject != instancedObjects.end(); instancedObject++)
	{
		auto buffer = m_BuffersByMesh.find(instancedObject->first);
		if (buffer == m_BuffersByMesh.end())
		{
			continue;
		}

		size_t objectsSize = instancedObject->second.size();
		if (objectsSize == 0)
		{
			continue;
		}

		if (buffer->second.m_PrevObjectSize != objectsSize || buffer->second.m_VertAttrib == nullptr)
		{
			delete[] buffer->second.m_VertAttrib;
			buffer->second.m_Size = objectsSize * m_SizeOfAttribs;
			buffer->second.m_VertAttrib = new float[buffer->second.m_Size];
		}

		buffer->second.m_PrevObjectSize = objectsSize;

		if (objectsSize < m_SizeOfObjectToThreads)
		{
			for (size_t i = 0; i < objectsSize; i++)
			{
				int m_TextureIndex = 0;
				bool isFound = false;
				Renderer3D& r3d = *(instancedObject->second[i]);
				Material& material = *r3d.m_Material;

				if (material.m_BaseColor != TextureManager::GetInstance().White())
				{
					auto instancedObject = std::find(buffer->second.m_TextureSlots.begin(), buffer->second.m_TextureSlots.end(), material.m_BaseColor->GetRendererID());
					if (instancedObject != buffer->second.m_TextureSlots.end())
					{
						m_TextureIndex = (instancedObject - buffer->second.m_TextureSlots.begin());
						isFound = true;
					}
					if (!isFound)
					{
						if (buffer->second.m_TextureSlotsIndex >= 32)
						{
							Logger::Warning("limit of texture slots, texture will be set to white!");
						}
						else
						{
							m_TextureIndex = buffer->second.m_TextureSlotsIndex;
							buffer->second.m_TextureSlots[buffer->second.m_TextureSlotsIndex] = material.m_BaseColor->GetRendererID();
							buffer->second.m_TextureSlotsIndex++;
						}
					}
				}

				GameObject& owner = *(r3d.GetOwner());

				glm::vec3 ambient = material.m_Ambient * material.m_Scale;
				glm::vec3 diffuse = material.m_Diffuse * material.m_Scale;
				glm::vec3 specular = material.m_Specular * material.m_Scale;

				float* startPtr = &buffer->second.m_VertAttrib[i * m_SizeOfAttribs];
				memcpy(startPtr, &owner.m_Transform.GetPosition()[0], 12);
				memcpy(&startPtr[3], &owner.m_Transform.GetScale()[0], 12);
				memcpy(&startPtr[6], &ambient[0], 12);
				memcpy(&startPtr[9], &diffuse[0], 12);
				memcpy(&startPtr[12], &specular[0], 12);
				startPtr[15] = (float)m_TextureIndex;
				startPtr[16] = material.m_Shininess;
				const float* view = glm::value_ptr(owner.m_Transform.GetRotationMat4());
				memcpy(&startPtr[17], view, 64);
			}

			continue;
		}

		for (size_t i = 0; i < m_NThreads; i++)
		{
			m_SyncParams.m_Ready[i] = true;
		}

		int threads = objectsSize > m_NThreads ? m_NThreads : objectsSize;
		float dif = (float)objectsSize / (float)threads;
		for (int k = 0; k < threads - 1; k++)
		{
			//glm::vec4 color;
			//if (k % 2 == 0)
			//{
			//	color = glm::vec4((float)k / (float)m_NThreads);
			//	color[1] = 1.0f;
			//	color[3] = 1.0f;
			//}
			//else
			//{
			//	color = glm::vec4((float)k / (float)m_NThreads);
			//	color[0] = 1.0f;
			//	color[3] = 1.0f;
			//}

			//if (k == 0)
			//	color = COLORS::Red;
			//else if (k == 1)
			//	color = COLORS::Green;
			//else if (k == 2)
			//	color = COLORS::Yellow;
			//else if (k == 3)
			//	color = COLORS::Brown;
			//else if (k == 4)
			//	color = COLORS::Silver;
			//else if (k == 5)
			//	color = COLORS::DarkGray;
			//else if (k == 6)
			//	color = COLORS::Orange;

			if ((size_t)(k * dif) >= (size_t)(k * dif + dif))
			{
				break;
			}

			ThreadPool::GetInstance().Enqueue([=] {
				m_SyncParams.m_Ready[k] = false;
				size_t thisSegmentOfObjectsV = k * dif + dif;
				for (size_t i = k * dif; i < thisSegmentOfObjectsV; i++)
				{
					int m_TextureIndex = 0;
					bool isFound = false;
					Renderer3D& r3d = *(instancedObject->second[i]);
					Material& material = *r3d.m_Material;

					if (material.m_BaseColor != TextureManager::GetInstance().White())
					{
						auto instancedObject = std::find(buffer->second.m_TextureSlots.begin(), buffer->second.m_TextureSlots.end(), material.m_BaseColor->GetRendererID());
						if (instancedObject != buffer->second.m_TextureSlots.end())
						{
							m_TextureIndex = (instancedObject - buffer->second.m_TextureSlots.begin());
							isFound = true;
						}
						if (!isFound)
						{
							if (buffer->second.m_TextureSlotsIndex >= 32)
							{
								Logger::Warning("limit of texture slots, texture will be set to white!");
							}
							else
							{
								//std::lock_guard lg(m_SyncParams.m_Mtx);
								m_TextureIndex = buffer->second.m_TextureSlotsIndex;
								buffer->second.m_TextureSlots[buffer->second.m_TextureSlotsIndex] = material.m_BaseColor->GetRendererID();
								buffer->second.m_TextureSlotsIndex++;
							}
						}
					}

					GameObject& owner = *(r3d.GetOwner());

					glm::vec3 ambient = material.m_Ambient * material.m_Scale;
					glm::vec3 diffuse = material.m_Diffuse * material.m_Scale;
					glm::vec3 specular = material.m_Specular * material.m_Scale;

					float* startPtr = &buffer->second.m_VertAttrib[i * m_SizeOfAttribs];
					memcpy(startPtr, &owner.m_Transform.GetPosition()[0], 12);
					memcpy(&startPtr[3], &owner.m_Transform.GetScale()[0], 12);
					memcpy(&startPtr[6], &ambient[0], 12);
					memcpy(&startPtr[9], &diffuse[0], 12);
					memcpy(&startPtr[12], &specular[0], 12);
					startPtr[15] = (float)m_TextureIndex;
					startPtr[16] = material.m_Shininess;
					const float* view = glm::value_ptr(owner.m_Transform.GetRotationMat4());
					memcpy(&startPtr[17], view, 64);
				}

				m_SyncParams.SetThreadFinished(k);
			});
		}

		if ((size_t)((threads - 1) * dif) >= objectsSize)
		{
			return;
		}

		ThreadPool::GetInstance().Enqueue([=] {
			m_SyncParams.m_Ready[threads - 1] = false;
			for (int i = (threads - 1) * dif; i < objectsSize; i++)
			{
				int m_TextureIndex = 0;
				bool isFound = false;
				Renderer3D& r3d = *(instancedObject->second[i]);
				Material& material = *r3d.m_Material;

				if (material.m_BaseColor != TextureManager::GetInstance().White())
				{
					auto instancedObject = std::find(buffer->second.m_TextureSlots.begin(), buffer->second.m_TextureSlots.end(), material.m_BaseColor->GetRendererID());
					if (instancedObject != buffer->second.m_TextureSlots.end())
					{
						m_TextureIndex = (instancedObject - buffer->second.m_TextureSlots.begin());
						isFound = true;
					}
					if (!isFound)
					{
						if (buffer->second.m_TextureSlotsIndex >= 32)
						{
							Logger::Warning("limit of texture slots, texture will be set to white!");
						}
						else
						{
							//std::lock_guard lg(m_SyncParams.m_Mtx);
							m_TextureIndex = buffer->second.m_TextureSlotsIndex;
							buffer->second.m_TextureSlots[buffer->second.m_TextureSlotsIndex] = material.m_BaseColor->GetRendererID();
							buffer->second.m_TextureSlotsIndex++;
						}
					}
				}

				GameObject& owner = *(r3d.GetOwner());

				glm::vec3 ambient = material.m_Ambient * material.m_Scale;
				glm::vec3 diffuse = material.m_Diffuse * material.m_Scale;
				glm::vec3 specular = material.m_Specular * material.m_Scale;

				float* startPtr = &buffer->second.m_VertAttrib[i * m_SizeOfAttribs];
				memcpy(startPtr, &owner.m_Transform.GetPosition()[0], 12);
				memcpy(&startPtr[3], &owner.m_Transform.GetScale()[0], 12);
				memcpy(&startPtr[6], &ambient[0], 12);
				memcpy(&startPtr[9], &diffuse[0], 12);
				memcpy(&startPtr[12], &specular[0], 12);
				startPtr[15] = (float)m_TextureIndex;
				startPtr[16] = material.m_Shininess;
				const float* view = glm::value_ptr(owner.m_Transform.GetRotationMat4());
				memcpy(&startPtr[17], view, 64);
			}

			m_SyncParams.SetThreadFinished(threads - 1);
		});
	}
}

void Instancing::ShutDown()
{
	for (auto buffer : m_BuffersByMesh)
	{
		delete buffer.second.m_VertAttrib;
	}
}
