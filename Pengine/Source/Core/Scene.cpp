#include "Scene.h"

#include "Timer.h"
#include "Logger.h"
#include "Viewport.h"
#include "Input.h"
#include "Editor.h"
#include "Visualizer.h"
#include "Environment.h"
#include "Raycast3D.h"
#include "../Lua/LuaStateManager.h"
#include "../Components/Renderer2D.h"
#include "../Components/Renderer3D.h"
#include "../Components/Rigidbody2D.h"
#include "../Components/BoxCollider2D.h"
#include "../Components/ParticleEmitter.h"

using namespace Pengine;

void Scene::Copy(const Scene& scene)
{
	m_Title = scene.m_Title;
	m_FilePath = scene.m_FilePath;

	delete m_Box2DWorld;
	m_Box2DWorld = new b2World(scene.m_Box2DWorld->GetGravity());

	for (const auto& gameObjectIter : scene.m_GameObjects)
	{
		if (gameObjectIter->GetOwner() == nullptr)
		{
			GameObject* gameObject = CreateGameObject(gameObjectIter->m_Name);
			*gameObject = *gameObjectIter;
		}
	}
}

void Scene::OnPhysicsUpdate()
{
	for (auto& gameObjectIter : m_GameObjects)
	{
		if (!gameObjectIter->IsEnabled()) continue;

		Rigidbody2D* rb2d = gameObjectIter->m_ComponentManager.GetComponent<Rigidbody2D>();
		if (rb2d)
		{
			rb2d->Initialize();

			if (!rb2d->IsInitializedPhysics()) continue;

			const glm::vec3 position = rb2d->GetOwner()->m_Transform.GetPosition();
			const glm::vec3 scale = rb2d->GetOwner()->m_Transform.GetScale();
			rb2d->m_Body->SetTransform({ position.x, position.y }, rb2d->GetOwner()->m_Transform.GetRotation().z);

			BoxCollider2D* bc2d = gameObjectIter->m_ComponentManager.GetComponent<BoxCollider2D>();
			if (bc2d)
			{
				if (b2Fixture* fixture = rb2d->m_Body->GetFixtureList())
				{
					rb2d->m_Body->SetTransform({ bc2d->GetPosition().x, bc2d->GetPosition().y }, bc2d->GetOwner()->m_Transform.GetRotation().z);
					fixture->SetDensity(bc2d->m_Density);
					fixture->SetFriction(bc2d->m_Friction);
					fixture->SetRestitution(bc2d->m_Restitution);
					fixture->SetRestitutionThreshold(bc2d->m_RestitutionThreshold);
					static_cast<b2PolygonShape*>(fixture->GetShape())->SetAsBox(bc2d->GetSize().x * scale.x, bc2d->GetSize().y * scale.y);
				}
			}

			CircleCollider2D* cc2d = gameObjectIter->m_ComponentManager.GetComponent<CircleCollider2D>();
			if (cc2d)
			{
				if (b2Fixture* fixture = rb2d->m_Body->GetFixtureList())
				{
					rb2d->m_Body->SetTransform({ cc2d->GetPosition().x, cc2d->GetPosition().y }, cc2d->GetOwner()->m_Transform.GetRotation().z);
					fixture->SetDensity(cc2d->m_Density);
					fixture->SetFriction(cc2d->m_Friction);
					fixture->SetRestitution(cc2d->m_Restitution);
					fixture->SetRestitutionThreshold(cc2d->m_RestitutionThreshold);
					static_cast<b2PolygonShape*>(fixture->GetShape())->m_radius = cc2d->GetRadius() * scale.x;
				}
			}
		}
	}

	const int32_t velocityIterations = 12;
	const int32_t positionIterations = 4;
	m_Box2DWorld->Step(Time::GetDeltaTime(), velocityIterations, positionIterations);

	for (auto& gameObjectIter : m_GameObjects)
	{
		if (!gameObjectIter->IsEnabled()) continue;

		Rigidbody2D* rb2d = gameObjectIter->m_ComponentManager.GetComponent<Rigidbody2D>();
		if (rb2d)
		{
			b2Body* body = rb2d->m_Body;
			const auto position = body->GetPosition();
			glm::vec2 offset = { 0.0f, 0.0f };
			if (ICollider2D* c2d = gameObjectIter->m_ComponentManager.GetComponent<ICollider2D>())
			{
				offset = c2d->GetOffset();

				gameObjectIter->m_Transform.Translate(glm::vec3(position.x - offset.x, position.y - offset.y, gameObjectIter->m_Transform.GetPosition().z));
				gameObjectIter->m_Transform.Rotate(glm::vec3(0.0f, 0.0f, body->GetAngle()));
			}
		}
	}
}

Scene::Scene(const std::string& title) : m_Title(title)
{
	m_Box2DWorld = new b2World(b2Vec2(0.0f, -9.8f));
	m_Renderer2DLayers.resize(10);
}

Scene::Scene(const Scene& scene)
{
	Copy(scene);
}

std::vector<GameObject*> Scene::SelectGameObject(std::vector<GameObject*> ignoreMask, int maxGameObjects)
{
	std::vector<GameObject*> gameObjects;

	if (Environment::GetInstance().GetMainCamera()->GetType() == Camera::CameraType::ORTHOGRAPHIC)
	{
		for (auto& gameObject : m_GameObjects)
		{
			if (!gameObject->m_IsSelectable) continue;

			const float rect[8] =
			{
				-0.5f, -0.5f,
				 0.5f, -0.5f,
				 0.5f,  0.5f,
				-0.5f,  0.5f
			};

			const glm::vec4 mousePosition = glm::inverse(gameObject->m_Transform.GetTransform())
				* glm::vec4(Viewport::GetInstance().GetMousePosition(), 0.0f, 1.0f);
			if (Utils::IsPointInsideRect(mousePosition, rect))
			{
				if (!Utils::IsThere(ignoreMask, gameObject))
				{
					gameObjects.push_back(gameObject);

					if (maxGameObjects == gameObjects.size())
					{
						return gameObjects;
					}
				}
			}
		}
	}
	else if (Environment::GetInstance().GetMainCamera()->GetType() == Camera::CameraType::PERSPECTIVE)
	{
		Raycast3D::Hit3D hit;
		std::map<float, class GameObject*> gameObjectsByDistance;
		std::vector<GameObject*> gameObjectsToRayCast;

		for (auto instancedObject : m_InstancedObjects)
		{
			for (Renderer3D* r3d : instancedObject.second)
			{
				gameObjectsToRayCast.push_back(r3d->GetOwner());
			}
		}
		
		const glm::vec3 start = Environment::GetInstance().GetMainCamera()->m_Transform.GetPosition();
		const glm::vec3 direction = Environment::GetInstance().GetMainCamera()->GetMouseDirection();
		gameObjectsByDistance = Raycast3D::RayCast(gameObjectsToRayCast, start, direction, hit, FLT_MAX, false);

		std::map<float, class GameObject*> gameObjectsByDistanceToTriangle;

		for (auto gameObjectByDistance : gameObjectsByDistance)
		{
			if (Renderer3D* r3d = gameObjectByDistance.second->m_ComponentManager.GetComponent<Renderer3D>())
			{
				const std::vector<float>& vertexAttributes = r3d->m_Mesh->GetVertexAttributes();
				const size_t vertexOffset = Mesh::GetVertexOffset();

				const std::vector<uint32_t>& indices = r3d->m_Mesh->GetIndices();
				const size_t indicesSize = indices.size();

				for (size_t i = 0; i < indicesSize; i+=3)
				{
					const size_t i0 = indices[i + 0] * vertexOffset;
					const size_t i1 = indices[i + 1] * vertexOffset;
					const size_t i2 = indices[i + 2] * vertexOffset;

					glm::vec3 a = { vertexAttributes[i0 + 0], vertexAttributes[i0 + 1], vertexAttributes[i0 + 2] };
					glm::vec3 b = { vertexAttributes[i1 + 0], vertexAttributes[i1 + 1], vertexAttributes[i1 + 2] };
					glm::vec3 c = { vertexAttributes[i2 + 0], vertexAttributes[i2 + 1], vertexAttributes[i2 + 2] };

					/*glm::vec3 n1 = { vertexAttributes[i0 + 3], vertexAttributes[i0 + 4], vertexAttributes[i0 + 5] };
					glm::vec3 n2 = { vertexAttributes[i1 + 3], vertexAttributes[i1 + 4], vertexAttributes[i1 + 5] };
					glm::vec3 n3 = { vertexAttributes[i2 + 3], vertexAttributes[i2 + 4], vertexAttributes[i2 + 5] };*/

					const glm::mat4 transform = gameObjectByDistance.second->m_Transform.GetTransform();
					const glm::mat4 rotation = gameObjectByDistance.second->m_Transform.GetRotationMat4();
					const glm::vec3 position = gameObjectByDistance.second->m_Transform.GetPosition();

					a = glm::vec3(transform * glm::vec4(a, 1.0f));
					b = glm::vec3(transform * glm::vec4(b, 1.0f));
					c = glm::vec3(transform * glm::vec4(c, 1.0f));

					glm::vec3 n = glm::normalize(glm::cross((b - a), (c - a)));

					if (Raycast3D::IntersectTriangle(start, direction, hit, FLT_MAX, a, b, c, n))
					{
						gameObjectsByDistanceToTriangle.emplace(hit.m_Distance, gameObjectByDistance.second);

						/*Visualizer::DrawLine(a, b, Colors::Yellow(), 2.0f);
						Visualizer::DrawLine(b, c, Colors::Yellow(), 2.0f);
						Visualizer::DrawLine(c, a, Colors::Yellow(), 2.0f);

						Visualizer::DrawLine(a, n1 + a, Colors::Blue());
						Visualizer::DrawLine(b, n2 + b, Colors::Blue());
						Visualizer::DrawLine(c, n3 + c, Colors::Blue());

						Visualizer::DrawLine({}, n, Colors::Red());

						Visualizer::DrawLine(start, start + direction, Colors::Blue(), 2.0f);*/

						break;
					}
				}
			}
		}

		for (auto gameObjectByDistanceToTriangle : gameObjectsByDistanceToTriangle)
		{
			gameObjects.push_back(gameObjectByDistanceToTriangle.second);

			if (maxGameObjects == gameObjects.size())
			{
				return gameObjects;
			}
		}
	}

	return gameObjects;
}

void Scene::OnRegisterClients()
{
	for (auto& gameObject : m_GameObjects)
	{
		for (auto& component : gameObject->m_ComponentManager.GetComponents())
		{
			component->OnRegisterClient();
		}
	}
}

void Scene::Render()
{
	const size_t size = m_Renderer2DLayers.size();
	if (size == 0) return;
	for (int i = size - 1; i >= 0; i--)
	{
		for (auto& r2d : m_Renderer2DLayers[i])
		{
			if(r2d->GetOwner()->m_IsEnabled)
			{
				r2d->Render();
			}
		}
	}

	for (auto& r3d : m_Renderers3D)
	{
		if (r3d->GetOwner()->m_IsEnabled)
		{
			r3d->Render();
		}
	}

	for (auto& particleEmitter : m_ParticleEmitters)
	{
		if (particleEmitter->GetOwner()->m_IsEnabled)
		{
			particleEmitter->Render();
		}
	}
}

void Scene::PrepareVisualizer()
{
	if (Editor::GetInstance().m_DrawColliders)
	{
		for (auto& gameObject : m_GameObjects)
		{
			if (gameObject->m_IsEnabled)
			{
				if (const BoxCollider2D* bc2d = gameObject->m_ComponentManager.GetComponent<BoxCollider2D>())
				{
					const glm::vec2 size = bc2d->GetSize();

					Transform transform = bc2d->GetOwner()->m_Transform;
					transform.Translate({ bc2d->GetPosition(), 0.0f });
					const glm::mat4 transformMat = transform.GetTransform();
					const glm::vec4 a = transformMat * glm::vec4(-size.x, -size.y, 0.0f, 1.0f);
					const glm::vec4 b = transformMat * glm::vec4(size.x, -size.y, 0.0f, 1.0f);
					const glm::vec4 c = transformMat * glm::vec4(size.x, size.y, 0.0f, 1.0f);
					const glm::vec4 d = transformMat * glm::vec4(-size.x, size.y, 0.0f, 1.0f);

					Visualizer::DrawLine(a, b, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
					Visualizer::DrawLine(b, c, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
					Visualizer::DrawLine(c, d, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
					Visualizer::DrawLine(d, a, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
				}

				if (const CircleCollider2D* cc2d = gameObject->m_ComponentManager.GetComponent<CircleCollider2D>())
				{
					Transform transform = cc2d->GetOwner()->m_Transform;
					transform.Translate({ cc2d->GetPosition(), 0.0f });
					const float radius = cc2d->GetRadius() * gameObject->m_Transform.GetScale().x;
					Visualizer::DrawCircle(radius - Editor::GetInstance().m_LineWidth * 0.01f, radius,
						transform.GetPositionMat4() * glm::scale(glm::mat4(1.0f), glm::vec3(radius * 2.0f)),
						glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
				}
			}
		}
	}

	if (Editor::GetInstance().m_DrawBoundingBoxes)
	{
		RenderBoundingBoxes();
	}

	if (Editor::GetInstance().m_DrawLights)
	{
		std::shared_ptr<Camera> camera = Environment::GetInstance().GetMainCamera();

		for (DirectionalLight* directionalLight : m_DirectionalLights)
		{
			Transform& transform = directionalLight->GetOwner()->m_Transform;
			glm::mat4 view = glm::inverse(glm::lookAt(glm::vec3(0.0f), camera->m_Transform.GetPosition() - transform.GetPosition(), glm::vec3(0.0f, 1.0f, 0.0f)));;
			Visualizer::DrawQuad(transform.GetPositionMat4() * view * transform.GetScaleMat4(), Colors::White(),
				TextureManager::GetInstance().GetByFilePath("Source/UIimages/Sun.png"));
		}

		for (SpotLight* spotLight : m_SpotLights)
		{
			Transform& transform = spotLight->GetOwner()->m_Transform;
			glm::mat4 view = glm::inverse(glm::lookAt(glm::vec3(0.0f), camera->m_Transform.GetPosition() - transform.GetPosition(), glm::vec3(0.0f, 1.0f, 0.0f)));;
			Visualizer::DrawQuad(transform.GetPositionMat4() * view * transform.GetScaleMat4(), Colors::White(),
				TextureManager::GetInstance().GetByFilePath("Source/UIimages/SpotLight.png"));
		}

		for (PointLight* pointLight : m_PointLights)
		{
			Transform& transform = pointLight->GetOwner()->m_Transform;
			glm::mat4 view = glm::inverse(glm::lookAt(glm::vec3(0.0f), camera->m_Transform.GetPosition() - transform.GetPosition(), glm::vec3(0.0f, 1.0f, 0.0f)));;
			Visualizer::DrawQuad(transform.GetPositionMat4() * view * transform.GetScaleMat4(), Colors::White(),
				TextureManager::GetInstance().GetByFilePath("Source/UIimages/PointLight.png"));
		}
	}
}

void Scene::RenderBoundingBoxes()
{
	for (const IRenderer* ir : m_Renderers3D)
	{
		const Renderer3D* r3d = (Renderer3D*)ir;
		if (const Mesh* mesh = r3d->m_Mesh)
		{
			const Transform& transform = r3d->GetOwner()->m_Transform;
			Visualizer::DrawWireFrameCube(transform.GetPositionMat4(), transform.GetRotationMat4(), transform.GetScaleMat4(),
				mesh->m_BoundingBox.m_Min, mesh->m_BoundingBox.m_Max);
		}
	}

	for (auto instancedObjectIter : m_InstancedObjects)
	{
		for (const Renderer3D* r3d : instancedObjectIter.second)
		{
			if (Mesh* mesh = r3d->m_Mesh)
			{
				const Transform& transform = r3d->GetOwner()->m_Transform;
				Visualizer::DrawWireFrameCube(transform.GetPositionMat4(), transform.GetRotationMat4(), transform.GetScaleMat4(),
					mesh->m_BoundingBox.m_Min, mesh->m_BoundingBox.m_Max);
			}
		}
	}
}

void Scene::ShutDown()
{
	Clear();
	delete m_Box2DWorld;
}

void Scene::Clear()
{
	m_Title = "Scene";
	m_FilePath = "None";

	while (m_GameObjects.size() > 0)
	{
		DeleteGameObject(m_GameObjects.front());
	}

	LuaStateManager::GetInstance().ShutDown();
}

Scene::Scene(Scene&& scene) noexcept
	: m_Title(std::move(scene.m_Title))
	, m_Box2DWorld(std::move(scene.m_Box2DWorld))
{
	scene.m_Box2DWorld = nullptr;
}

void Scene::operator=(const Scene& scene)
{
	Copy(scene);
}

GameObject* Scene::CreateGameObject(const std::string& name, const Transform& transform, const UUID& uuid)
{
	return GameObject::Create(this, name, transform, uuid);
}

void Scene::DeleteGameObject(GameObject* gameObject)
{
	gameObject->Delete();
}

void Scene::DeleteGameObjectLater(GameObject* gameObject)
{
	gameObject->DeleteLater();
}

GameObject* Scene::FindGameObject(const std::string& name)
{
	auto gameObjectIter = std::find_if(m_GameObjects.begin(), m_GameObjects.end(), [name](GameObject* gameObject) {
		return gameObject->GetName() == name;
	});

	if (gameObjectIter != m_GameObjects.end())
	{
		return *gameObjectIter;
	}

	return nullptr;
}

std::vector<GameObject*> Scene::FindGameObjects(const std::string& name)
{
	std::vector<GameObject*> gameObjects;
	for (auto gameObject : m_GameObjects)
	{
		if (gameObject->m_Name == name)
		{
			gameObjects.push_back(gameObject);
		}
	}
	return gameObjects;
}

GameObject* Scene::FindGameObject(size_t uuid)
{
	auto gameObjectIter = std::find_if(m_GameObjects.begin(), m_GameObjects.end(), [uuid](GameObject* gameObject) {
		return gameObject->GetUUID().operator size_t() == uuid;
	});

	if (gameObjectIter != m_GameObjects.end())
	{
		return *gameObjectIter;
	}

	return nullptr;
}
