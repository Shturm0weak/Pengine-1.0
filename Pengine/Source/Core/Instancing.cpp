#include "Instancing.h"

#include "Environment.h"
#include "Editor.h"

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
	m_Buffers.insert(std::make_pair(mesh, glBuffers()));
	m_Buffers.rbegin()->second.m_VboDynamic.Initialize(nullptr, 1, false);
}

void Instancing::Render(const std::map<Mesh*, std::vector<Renderer3D*>>& instancedObjects)
{
	Shader* shader = Shader::Get("Instancing3D");

	for (auto iter = instancedObjects.begin(); iter != instancedObjects.end(); iter++)
	{
		auto gliter = m_Buffers.find(iter->first);
		if (gliter == m_Buffers.end())
			continue;

		size_t objectsSize = iter->second.size();
		if (objectsSize == 0)
			continue;

		shader->Bind();
		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, TextureManager::GetInstance().White()->GetRendererID());
		gliter->second.m_TextureSlots[0] = TextureManager::GetInstance().White()->GetRendererID();

		Environment& environment = Environment::GetInstance();

		shader->SetUniformMat4f("u_ViewProjection", environment.GetMainCamera()->GetViewProjectionMat4());
		shader->SetUniform3fv("u_CameraPosition", environment.GetMainCamera()->m_Transform.GetPosition());

		if (iter->second[0]->GetOwner()->GetScene()->m_DirectionalLight.empty())
		{
			shader->SetUniform1f("u_DirectionalLight.intensity", 0.0f);
		}
		else
		{
			DirectionalLight* directionalLight = iter->second[0]->GetOwner()->GetScene()->m_DirectionalLight[0];

			shader->SetUniform3fv("u_DirectionalLight.direction", directionalLight->GetOwner()->m_Transform.GetRotationMat4() * glm::vec4(0, 0, -1, 1));
			shader->SetUniform3fv("u_DirectionalLight.color", directionalLight->m_Color);
			shader->SetUniform1f("u_DirectionalLight.intensity", directionalLight->m_Intensity);
		}
		
		Editor::GetInstance().m_Stats.m_Vertices += gliter->first->GetIndices().size() * objectsSize;
		Editor::GetInstance().m_Stats.m_DrawCalls++;

		if (objectsSize > m_SizeOfObjectToThreads - 1)
		{
			m_SyncParams.WaitForAllThreads();
		}

		gliter->first->m_Va.Bind();
		gliter->first->m_Ib.Bind();
		gliter->first->m_Vb.Bind();
		gliter->second.m_VboDynamic.Invalidate();
		gliter->second.m_VboDynamic.Initialize(gliter->second.m_VertAttrib, 4 * m_SizeOfAttribs * objectsSize, false);
		gliter->second.m_LayoutDynamic.Clear();
		gliter->second.m_LayoutDynamic.Push<float>(3);
		gliter->second.m_LayoutDynamic.Push<float>(3);
		gliter->second.m_LayoutDynamic.Push<float>(3);
		gliter->second.m_LayoutDynamic.Push<float>(3);
		gliter->second.m_LayoutDynamic.Push<float>(3);
		gliter->second.m_LayoutDynamic.Push<float>(2);
		gliter->second.m_LayoutDynamic.Push<float>(4);
		gliter->second.m_LayoutDynamic.Push<float>(4);
		gliter->second.m_LayoutDynamic.Push<float>(4);
		gliter->second.m_LayoutDynamic.Push<float>(4);
		gliter->first->m_Va.AddBuffer(gliter->second.m_VboDynamic, gliter->second.m_LayoutDynamic, 4, 1);

		int samplers[32];
		for (unsigned int i = 1; i < gliter->second.m_TextureSlotsIndex; i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, gliter->second.m_TextureSlots[i]);
		}
		for (unsigned int i = 0; i < maxTextureSlots; i++)
		{
			samplers[i] = i;
		}

		shader->SetUniform1iv("u_Texture", samplers);

		if (!iter->second[0]->m_BackFaceCulling)
		{
			glDisable(GL_CULL_FACE);
		}

		if (Editor::GetInstance().m_PolygonMode)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}

		glDrawElementsInstanced(GL_TRIANGLES, gliter->first->m_Ib.GetCount(), GL_UNSIGNED_INT, 0, iter->second.size());

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_CULL_FACE);

		shader->UnBind();
		gliter->first->m_Ib.UnBind();
		gliter->first->m_Va.UnBind();

		gliter->second.m_TextureSlotsIndex = 1;
		for (unsigned int i = gliter->second.m_TextureSlotsIndex; i < 32; i++)
		{
			gliter->second.m_TextureSlots[i] = 0;
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, gliter->second.m_TextureSlots[0]);
		}

		glDisable(GL_TEXTURE_2D_ARRAY);
	}
}

void Instancing::PrepareVertexAtrrib(const std::map<Mesh*, std::vector<Renderer3D*>>& instancedObjects)
{
	for (auto iter = instancedObjects.begin(); iter != instancedObjects.end(); iter++)
	{
		auto gliter = m_Buffers.find(iter->first);
		if (gliter == m_Buffers.end()) continue;

		size_t objectsSize = iter->second.size();
		if (objectsSize == 0) continue;

		if (gliter->second.m_PrevObjectSize != objectsSize || gliter->second.m_VertAttrib == nullptr)
		{
			delete[] gliter->second.m_VertAttrib;
			gliter->second.m_VertAttrib = nullptr;
			gliter->second.m_VertAttrib = new float[objectsSize * m_SizeOfAttribs];
		}

		gliter->second.m_PrevObjectSize = objectsSize;

		if (objectsSize < m_SizeOfObjectToThreads)
		{
			for (size_t i = 0; i < objectsSize; i++)
			{
				int m_TextureIndex = 0;
				Renderer3D& r3d = *(iter->second[i]);

				bool isFound = false;
				if (r3d.m_Material.m_BaseColor != TextureManager::GetInstance().White())
				{
					auto iter = std::find(gliter->second.m_TextureSlots.begin(), gliter->second.m_TextureSlots.end(), r3d.m_Material.m_BaseColor->GetRendererID());
					if (iter != gliter->second.m_TextureSlots.end())
					{
						m_TextureIndex = (iter - gliter->second.m_TextureSlots.begin());
						isFound = true;
					}
					if (!isFound)
					{
						if (gliter->second.m_TextureSlotsIndex >= 32)
						{
							Logger::Warning("limit of texture slots, texture will be set to white!");
						}
						else
						{
							m_TextureIndex = gliter->second.m_TextureSlotsIndex;
							gliter->second.m_TextureSlots[gliter->second.m_TextureSlotsIndex] = r3d.m_Material.m_BaseColor->GetRendererID();
							gliter->second.m_TextureSlotsIndex++;
						}
					}
				}

				GameObject& owner = *(r3d.GetOwner());

				glm::vec3 ambient = r3d.m_Material.m_Ambient * r3d.m_Material.m_Scale;
				glm::vec3 diffuse = r3d.m_Material.m_Diffuse * r3d.m_Material.m_Scale;
				glm::vec3 specular = r3d.m_Material.m_Specular * r3d.m_Material.m_Scale;

				float* posPtr = &gliter->second.m_VertAttrib[i * m_SizeOfAttribs];
				memcpy(posPtr, &owner.m_Transform.GetPosition()[0], 12);
				memcpy(&posPtr[3], &owner.m_Transform.GetScale()[0], 12);
				memcpy(&posPtr[6], &ambient[0], 12);
				memcpy(&posPtr[9], &diffuse[0], 12);
				memcpy(&posPtr[12], &specular[0], 12);
				posPtr[15] = (float)m_TextureIndex;
				posPtr[16] = r3d.m_Material.m_Shininess;
				const float* view = glm::value_ptr(owner.m_Transform.GetRotationMat4());
				memcpy(&posPtr[17], view, 64);
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
					Renderer3D& r3d = *(iter->second[i]);

					bool isFound = false;
					if (r3d.m_Material.m_BaseColor != TextureManager::GetInstance().White())
					{
						auto iter = std::find(gliter->second.m_TextureSlots.begin(), gliter->second.m_TextureSlots.end(), r3d.m_Material.m_BaseColor->GetRendererID());
						if (iter != gliter->second.m_TextureSlots.end())
						{
							m_TextureIndex = (iter - gliter->second.m_TextureSlots.begin());
							isFound = true;
						}
						if (!isFound)
						{
							if (gliter->second.m_TextureSlotsIndex >= 32)
							{
								Logger::Warning("limit of texture slots, texture will be set to white!");
							}
							else
							{
								//std::lock_guard lg(m_SyncParams.m_Mtx);
								m_TextureIndex = gliter->second.m_TextureSlotsIndex;
								gliter->second.m_TextureSlots[gliter->second.m_TextureSlotsIndex] = r3d.m_Material.m_BaseColor->GetRendererID();
								gliter->second.m_TextureSlotsIndex++;
							}
						}
					}

					GameObject& owner = *(r3d.GetOwner());

					glm::vec3 ambient = r3d.m_Material.m_Ambient * r3d.m_Material.m_Scale;
					glm::vec3 diffuse = r3d.m_Material.m_Diffuse * r3d.m_Material.m_Scale;
					glm::vec3 specular = r3d.m_Material.m_Specular * r3d.m_Material.m_Scale;

					float* posPtr = &gliter->second.m_VertAttrib[i * m_SizeOfAttribs];
					memcpy(posPtr, &owner.m_Transform.GetPosition()[0], 12);
					memcpy(&posPtr[3], &owner.m_Transform.GetScale()[0], 12);
					memcpy(&posPtr[6], &ambient[0], 12);
					memcpy(&posPtr[9], &diffuse[0], 12);
					memcpy(&posPtr[12], &specular[0], 12);
					posPtr[15] = (float)m_TextureIndex;
					posPtr[16] = r3d.m_Material.m_Shininess;
					const float* view = glm::value_ptr(owner.m_Transform.GetRotationMat4());
					memcpy(&posPtr[17], view, 64);
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
				Renderer3D& r3d = *(iter->second[i]);
				int m_TextureIndex = 0;

				bool isFound = false;
				if (r3d.m_Material.m_BaseColor != TextureManager::GetInstance().White())
				{
					auto iter = std::find(gliter->second.m_TextureSlots.begin(), gliter->second.m_TextureSlots.end(), r3d.m_Material.m_BaseColor->GetRendererID());
					if (iter != gliter->second.m_TextureSlots.end())
					{
						m_TextureIndex = (iter - gliter->second.m_TextureSlots.begin());
						isFound = true;
					}
					if (!isFound)
					{
						if (gliter->second.m_TextureSlotsIndex >= 32)
						{
							Logger::Warning("limit of texture slots, texture will be set to white!");
						}
						else
						{
							//std::lock_guard lg(m_SyncParams.m_Mtx);
							m_TextureIndex = gliter->second.m_TextureSlotsIndex;
							gliter->second.m_TextureSlots[gliter->second.m_TextureSlotsIndex] = r3d.m_Material.m_BaseColor->GetRendererID();
							gliter->second.m_TextureSlotsIndex++;
						}
					}
				}

				GameObject& owner = *(r3d.GetOwner());

				glm::vec3 ambient = r3d.m_Material.m_Ambient * r3d.m_Material.m_Scale;
				glm::vec3 diffuse = r3d.m_Material.m_Diffuse * r3d.m_Material.m_Scale;
				glm::vec3 specular = r3d.m_Material.m_Specular * r3d.m_Material.m_Scale;

				float* posPtr = &gliter->second.m_VertAttrib[i * m_SizeOfAttribs];
				memcpy(posPtr, &owner.m_Transform.GetPosition()[0], 12);
				memcpy(&posPtr[3], &owner.m_Transform.GetScale()[0], 12);
				memcpy(&posPtr[6], &ambient[0], 12);
				memcpy(&posPtr[9], &diffuse[0], 12);
				memcpy(&posPtr[12], &specular[0], 12);
				posPtr[15] = (float)m_TextureIndex;
				posPtr[16] = r3d.m_Material.m_Shininess;
				const float* view = glm::value_ptr(owner.m_Transform.GetRotationMat4());
				memcpy(&posPtr[17], view, 64);
			}
			
			m_SyncParams.SetThreadFinished(threads - 1);
		});
	}
}

void Instancing::ShutDown()
{
	//for (auto buffer : m_Buffers)
	//{
	//	delete buffer.second.m_LayoutDynamic;
	//	delete buffer.second.m_VboDynamic;
	//}
}
