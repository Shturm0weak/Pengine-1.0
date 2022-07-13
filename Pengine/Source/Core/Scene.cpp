#include "Scene.h"

#include "Timer.h"
#include "Logger.h"
#include "Viewport.h"
#include "Input.h"
#include "Editor.h"
#include "Visualizer.h"
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

void Scene::OnPhysicsStart()
{
	for (auto& gameObjectIter : m_GameObjects)
	{
		Rigidbody2D* rb2d = gameObjectIter->m_ComponentManager.GetComponent<Rigidbody2D>();
		if (rb2d)
		{
			rb2d->Initialize();
		}
	}
}

void Scene::OnPhysicsUpdate()
{
	for (auto& gameObjectIter : m_GameObjects)
	{
		Rigidbody2D* rb2d = gameObjectIter->m_ComponentManager.GetComponent<Rigidbody2D>();
		if (rb2d)
		{
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
		Rigidbody2D* rb2d = gameObjectIter->m_ComponentManager.GetComponent<Rigidbody2D>();
		if (rb2d)
		{
			b2Body* body = rb2d->m_Body;
			const auto position = body->GetPosition();
			glm::vec2 offset;
			if (ICollider2D* c2d = gameObjectIter->m_ComponentManager.GetComponent<ICollider2D>())
			{
				offset = c2d->GetOffset();
			}
			gameObjectIter->m_Transform.Translate(glm::vec3(position.x - offset.x, position.y - offset.y, 0.0f));
			gameObjectIter->m_Transform.Rotate(glm::vec3(0.0f, 0.0f, body->GetAngle()));
		}
	}
}

void Scene::OnPhysicsClose()
{
	// It has been already destroyed when a gameobject is destroyed.
	//for (auto& gameObjectIter : m_GameObjects)
	//{
	//	Rigidbody2D* rb2d = gameObjectIter->m_ComponentManager.GetComponent<Rigidbody2D>();
	//	if (rb2d)
	//	{
	//		rb2d->m_Body->DestroyFixture(rb2d->m_Fixture);
	//		m_Box2DWorld->DestroyBody(rb2d->m_Body);
	//	}
	//}
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

		glm::vec4 mousePosition = glm::inverse(gameObject->m_Transform.GetTransform())
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

	return gameObjects;
}

void Scene::Render()
{
	size_t size = m_Renderer2DLayers.size();
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

	if (Editor::GetInstance().m_DrawColliders)
	{
		for (auto& gameObject : m_GameObjects)
		{
			if (gameObject->m_IsEnabled)
			{
				if (BoxCollider2D* bc2d = gameObject->m_ComponentManager.GetComponent<BoxCollider2D>())
				{
					glm::vec2 size = bc2d->GetSize();

					Transform transform = bc2d->GetOwner()->m_Transform;
					transform.Translate({ bc2d->GetPosition(), 0.0f });
					glm::mat4 transformMat = transform.GetTransform();
					glm::vec4 a = transformMat * glm::vec4(-size.x, -size.y, 0.0f, 1.0f);
					glm::vec4 b = transformMat * glm::vec4( size.x, -size.y, 0.0f, 1.0f);
					glm::vec4 c = transformMat * glm::vec4( size.x,  size.y, 0.0f, 1.0f);
					glm::vec4 d = transformMat * glm::vec4(-size.x,  size.y, 0.0f, 1.0f);

					Visualizer::DrawLine(a, b, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
					Visualizer::DrawLine(b, c, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
					Visualizer::DrawLine(c, d, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
					Visualizer::DrawLine(d, a, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
				}

				if (CircleCollider2D* cc2d = gameObject->m_ComponentManager.GetComponent<CircleCollider2D>())
				{
					Transform transform = cc2d->GetOwner()->m_Transform;
					transform.Translate({ cc2d->GetPosition(), 0.0f });
					float radius = cc2d->GetRadius() * gameObject->m_Transform.GetScale().x;
					Visualizer::DrawCircle(radius * 0.95f, radius, transform.GetTransform(), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
				}
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
	while (m_GameObjects.size() > 0)
	{
		DeleteGameObject(m_GameObjects.back());
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
