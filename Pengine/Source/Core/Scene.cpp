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
#include "../OpenGL/Material.h"
#include "../OpenGL/BaseMaterial.h"

using namespace Pengine;

void Scene::Copy(const Scene& scene)
{
	m_Title = scene.m_Title;
	m_FilePath = scene.m_FilePath;

	DestroyPhysics();
	CreatePhysics();

	for (const auto& gameObject : scene.m_GameObjects)
	{
		if (gameObject->GetOwner() == nullptr)
		{
			GameObject* createdGameObject = CreateGameObject(gameObject->m_Name, gameObject->m_Transform, gameObject->m_UUID);
			*createdGameObject = *gameObject;
		}
	}
}

void Scene::OnPhysicsUpdate()
{
	for (Rigidbody2D* rb2d : m_Rigidbody2D)
	{
		GameObject* owner = rb2d->GetOwner();
		if (!owner->IsEnabled()) continue;

		rb2d->Initialize();

		if (!rb2d->IsInitializedPhysics()) continue;

		const glm::vec3 position = rb2d->GetOwner()->m_Transform.GetPosition();
		const glm::vec3 scale = rb2d->GetOwner()->m_Transform.GetScale();
		rb2d->m_Body->SetTransform({ position.x, position.y }, rb2d->GetOwner()->m_Transform.GetRotation().z);

		BoxCollider2D* bc2d = owner->m_ComponentManager.GetComponent<BoxCollider2D>();
		if (bc2d)
		{
			if (b2Fixture* fixture = rb2d->m_Body->GetFixtureList())
			{
				rb2d->m_Body->SetTransform({ bc2d->GetPosition().x, bc2d->GetPosition().y }, bc2d->GetOwner()->m_Transform.GetRotation().z);
				fixture->SetDensity(bc2d->GetDensity());
				fixture->SetFriction(bc2d->GetFriction());
				fixture->SetRestitution(bc2d->GetRestitution());
				fixture->SetRestitutionThreshold(bc2d->GetRestitutionThreshold());
				static_cast<b2PolygonShape*>(fixture->GetShape())->SetAsBox(bc2d->GetSize().x * scale.x, bc2d->GetSize().y * scale.y);
			}
		}

		CircleCollider2D* cc2d = owner->m_ComponentManager.GetComponent<CircleCollider2D>();
		if (cc2d)
		{
			if (b2Fixture* fixture = rb2d->m_Body->GetFixtureList())
			{
				rb2d->m_Body->SetTransform({ cc2d->GetPosition().x, cc2d->GetPosition().y }, cc2d->GetOwner()->m_Transform.GetRotation().z);
				fixture->SetDensity(cc2d->GetDensity());
				fixture->SetFriction(cc2d->GetFriction());
				fixture->SetRestitution(cc2d->GetRestitution());
				fixture->SetRestitutionThreshold(cc2d->GetRestitutionThreshold());
				static_cast<b2PolygonShape*>(fixture->GetShape())->m_radius = cc2d->GetRadius() * scale.x;
			}
		}
	}

	for (Rigidbody3D* rb3d : m_Rigidbody3D)
	{
		GameObject* owner = rb3d->GetOwner();
		if (!owner->IsEnabled()) continue;

		rb3d->Initialize();

		if (!rb3d->IsInitializedPhysics()) continue;

		if (!rb3d->m_AllowToSleep)
		{
			rb3d->SetActive(true);
		}

		glm::vec3 position = { 0.0f, 0.0f, 0.0f };
		if (ICollider3D* c3d = rb3d->GetCollisionShape())
		{
			position = c3d->GetPosition();
		}

		float mass;
		btVector3 inertia;
		rb3d->GetBody().setRestitution(rb3d->m_Restitution);
		rb3d->GetBody().setFriction(rb3d->m_Friction);
		rb3d->GetMassProps(mass, inertia);
		rb3d->GetBody().setMassProps(mass, inertia);
		btTransform transform;
		transform.setFromOpenGLMatrix(glm::value_ptr(glm::translate(glm::mat4(1.0f), position) *
			rb3d->GetOwner()->m_Transform.GetRotationMat4(Transform::System::LOCAL)));
		rb3d->GetBody().setWorldTransform(transform);
	}

	const int32_t velocityIterations = 12;
	const int32_t positionIterations = 4;
	m_Box2DWorld->Step(Time::GetDeltaTime(), velocityIterations, positionIterations);

	m_BulletPhysicsWorld.Step(Time::GetDeltaTime());

	for (Rigidbody2D* rb2d : m_Rigidbody2D)
	{
		GameObject* owner = rb2d->GetOwner();
		if (!owner->IsEnabled()) continue;

		b2Body* body = rb2d->m_Body;
		const auto position = body->GetPosition();
		glm::vec2 offset = { 0.0f, 0.0f };
		if (ICollider2D* c2d = owner->m_ComponentManager.GetComponentSubClass<ICollider2D>())
		{
			offset = c2d->GetOffset();

			owner->m_Transform.Translate(glm::vec3(position.x - offset.x, position.y - offset.y, owner->m_Transform.GetPosition().z));
			owner->m_Transform.Rotate(glm::vec3(0.0f, 0.0f, body->GetAngle()));
		}
	}

	for (Rigidbody3D* rb3d : m_Rigidbody3D)
	{
		GameObject* owner = rb3d->GetOwner();
		if (!owner->IsEnabled()) continue;

		const btRigidBody& body = rb3d->GetBody();
		const btVector3 position = body.getWorldTransform().getOrigin();
		glm::vec3 offset = { 0.0f, 0.0f, 0.0f };
		if (ICollider3D* c3d = rb3d->GetCollisionShape())
		{
			offset = c3d->GetOffset();
		}

		owner->m_Transform.Translate(glm::vec3(position.x() - offset.x,
			position.y() - offset.y, position.z() - offset.z) - owner->m_Transform.GetPosition() + owner->m_Transform.GetPosition(Transform::System::LOCAL));

		glm::vec3 rotation;
		body.getWorldTransform().getBasis().getEulerZYX(rotation.z, rotation.y, rotation.x);
		owner->m_Transform.Rotate(rotation - owner->m_Transform.GetRotation() + owner->m_Transform.GetRotation(Transform::System::LOCAL));
	}
}

Scene::Scene(const std::string& title) : m_Title(title)
{
	CreatePhysics();
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

		for (Renderer3D* r3d : m_Renderers3D)
		{
			if (r3d->GetOwner()->IsSelectable())
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

	for (auto& particleEmitter : m_ParticleEmitters)
	{
		if (particleEmitter->GetOwner()->m_IsEnabled)
		{
			particleEmitter->Render();
		}
	}

	for (auto& r3d : m_TransparentByDistance)
	{
		r3d->Render();
	}
}

void Scene::PrepareVisualizer()
{
	if (Editor::GetInstance().m_DrawColliders)
	{
		for (const BoxCollider2D* bc2d : m_BoxColliders2D)
		{
			if (!bc2d->GetOwner()->IsEnabled()) continue;

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

		std::shared_ptr<Camera> camera = Environment::GetInstance().GetMainCamera();
		for (const CircleCollider2D* cc2d : m_CircleColliders2D)
		{
			if (!cc2d->GetOwner()->IsEnabled()) continue;

			Transform transform = cc2d->GetOwner()->m_Transform;
			transform.Translate({ cc2d->GetPosition(), 0.0f });
			const float distance = glm::distance(camera->m_Transform.GetPosition(), transform.GetPosition());
			const float radius = cc2d->GetRadius() * cc2d->GetOwner()->m_Transform.GetScale().x;
			Visualizer::DrawCircle(radius - Editor::GetInstance().m_LineWidth * 0.005f * distance, radius,
				transform.GetPositionMat4() * glm::scale(glm::mat4(1.0f), glm::vec3(radius * 2.0f)),
				glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
		}

		for (const BoxCollider3D* bc3d : m_BoxColliders3D)
		{
			if (!bc3d->GetOwner()->IsEnabled()) continue;

			Transform transform;
			transform.CopyGlobal(bc3d->GetOwner()->m_Transform);
			transform.Translate(transform.GetPosition() + bc3d->GetOffset());
			Visualizer::DrawWireFrameCube(transform.GetPositionMat4(), transform.GetRotationMat4(),
				transform.GetScaleMat4(), -bc3d->GetHalfExtent(), bc3d->GetHalfExtent());
		}

		for (const SphereCollider3D* sc3d : m_SphereColliders3D)
		{
			if (!sc3d->GetOwner()->IsEnabled()) continue;

			Transform transform = sc3d->GetOwner()->m_Transform;
			transform.Translate(transform.GetPosition() + sc3d->GetOffset());
			const float distance = glm::distance(camera->m_Transform.GetPosition(), transform.GetPosition());
			const float radius = sc3d->GetRadius() * sc3d->GetOwner()->m_Transform.GetScale().x;
			Visualizer::DrawWireFrameSphere(4, 4, radius - Editor::GetInstance().m_LineWidth * 0.005f * distance, radius,
				transform.GetPositionMat4(), transform.GetRotationMat4(), glm::scale(glm::mat4(1.0f), glm::vec3(radius * 2.0f)),
				glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
		}
	}

	if (Editor::GetInstance().m_DrawBoundingBoxes)
	{
		RenderBoundingBoxes();
	}

	if (Editor::GetInstance().m_DrawLights)
	{
		const std::shared_ptr<Camera> camera = Environment::GetInstance().GetMainCamera();

		for (DirectionalLight* directionalLight : m_DirectionalLights)
		{
			const Transform& transform = directionalLight->GetOwner()->m_Transform;
			const glm::mat4 view = glm::inverse(glm::lookAt(glm::vec3(0.0f), camera->m_Transform.GetPosition() - transform.GetPosition(), glm::vec3(0.0f, 1.0f, 0.0f)));;
			Visualizer::DrawQuad(transform.GetPositionMat4() * view * transform.GetScaleMat4(), Colors::White(),
				TextureManager::GetInstance().GetByFilePath("Source/UIimages/Sun.png"));
		}

		for (SpotLight* spotLight : m_SpotLights)
		{
			const Transform& transform = spotLight->GetOwner()->m_Transform;
			const glm::mat4 view = glm::inverse(glm::lookAt(glm::vec3(0.0f), camera->m_Transform.GetPosition() - transform.GetPosition(), glm::vec3(0.0f, 1.0f, 0.0f)));;
			Visualizer::DrawQuad(transform.GetPositionMat4() * view * transform.GetScaleMat4(), Colors::White(),
				TextureManager::GetInstance().GetByFilePath("Source/UIimages/SpotLight.png"));
		}

		for (PointLight* pointLight : m_PointLights)
		{
			const Transform& transform = pointLight->GetOwner()->m_Transform;
			const glm::mat4 view = glm::inverse(glm::lookAt(glm::vec3(0.0f), camera->m_Transform.GetPosition() - transform.GetPosition(), glm::vec3(0.0f, 1.0f, 0.0f)));;
			Visualizer::DrawQuad(transform.GetPositionMat4() * view * transform.GetScaleMat4(), Colors::White(),
				TextureManager::GetInstance().GetByFilePath("Source/UIimages/PointLight.png"));
		}
	}
}

void Scene::PrepareToRender()
{
	m_OpaqueByBaseMaterial.clear();
	m_TransparentByDistance.clear();
	m_ShadowsByMesh.clear();

	const glm::vec3 cameraPosition = Environment::GetInstance().GetMainCamera()->m_Transform.GetPosition();

	for (Renderer3D* r3d : m_Renderers3D)
	{
		if (!r3d->GetOwner()->IsEnabled()) continue;

		r3d->SortLods(cameraPosition);

		if (Mesh* mesh = r3d->m_Mesh)
		{
			if (r3d->m_IsOpaque)
			{
				m_OpaqueByBaseMaterial[r3d->m_Material->m_BaseMaterial][mesh].emplace_back(r3d);

				if (r3d->m_DrawShadows)
				{
					m_ShadowsByMesh[mesh].emplace_back(r3d);
				}
			}
			else
			{
				m_TransparentByDistance.emplace_back(r3d);
			}
		}
	}
}

void Scene::SortTransparent()
{
	const glm::vec3 cameraPosition = Environment::GetInstance().GetMainCamera()->m_Transform.GetPosition();

	auto isFurther = [cameraPosition](Renderer3D* a, Renderer3D* b)
	{
		float distance2A = glm::length2(cameraPosition - a->m_Mesh->m_BoundingBox.GetTransformedCenter(a->GetOwner()->m_Transform.GetPosition()));
		float distance2B = glm::length2(cameraPosition - b->m_Mesh->m_BoundingBox.GetTransformedCenter(b->GetOwner()->m_Transform.GetPosition()));

		return distance2A > distance2B;
	};

	std::sort(m_TransparentByDistance.begin(), m_TransparentByDistance.end(), isFurther);
}

void Scene::SortPointLights()
{
	const glm::vec3 cameraPosition = Environment::GetInstance().GetMainCamera()->m_Transform.GetPosition();

	auto isCloser = [cameraPosition](PointLight* a, PointLight* b)
	{
		float distance2A = glm::length2(cameraPosition - a->GetOwner()->m_Transform.GetPosition());
		float distance2B = glm::length2(cameraPosition - b->GetOwner()->m_Transform.GetPosition());

		return distance2A < distance2B;
	};

	std::sort(m_PointLights.begin(), m_PointLights.end(), isCloser);

	for (PointLight* pointLight : m_PointLights)
	{
		float distance2 = glm::length2(cameraPosition - pointLight->GetOwner()->m_Transform.GetPosition());
		pointLight->m_ShadowsVisible = distance2 < pointLight->m_ZFar * pointLight->m_ZFar * 2.0f;
	}
}

void Scene::SortSpotLights()
{
	const glm::vec3 cameraPosition = Environment::GetInstance().GetMainCamera()->m_Transform.GetPosition();

	auto isCloser = [cameraPosition](SpotLight* a, SpotLight* b)
	{
		float distance2A = glm::length2(cameraPosition - a->GetOwner()->m_Transform.GetPosition());
		float distance2B = glm::length2(cameraPosition - b->GetOwner()->m_Transform.GetPosition());

		return distance2A < distance2B;
	};

	std::sort(m_SpotLights.begin(), m_SpotLights.end(), isCloser);

	for (SpotLight* spotLight : m_SpotLights)
	{
		float distance2 = glm::length2(cameraPosition - spotLight->GetOwner()->m_Transform.GetPosition());
		spotLight->m_ShadowsVisible = distance2 < spotLight->m_ZFar* spotLight->m_ZFar * 2.0f;
	}
}

void Scene::RenderBoundingBoxes()
{
	for (const Renderer3D* r3d : m_Renderers3D)
	{
		if (const Mesh* mesh = r3d->m_Mesh)
		{
			const Transform& transform = r3d->GetOwner()->m_Transform;
			Visualizer::DrawWireFrameCube(transform.GetPositionMat4(), transform.GetRotationMat4(Transform::System::LOCAL), transform.GetScaleMat4(Transform::System::LOCAL),
				mesh->m_BoundingBox.m_Min, mesh->m_BoundingBox.m_Max);
		}
	}
}

void Scene::ShutDown()
{
	Clear();
	DestroyPhysics();
}

void Scene::CreatePhysics()
{
	m_Box2DWorld = new b2World(b2Vec2(0.0f, -9.8f));
	m_BulletPhysicsWorld.Initialize();
}

void Scene::DestroyPhysics()
{
	delete m_Box2DWorld;
	m_BulletPhysicsWorld.UnInitialize();
}

std::vector<PointLight*> Scene::GetEnabledPointLights() const
{
	std::vector<PointLight*> pointLights;
	pointLights.reserve(10);
	for (size_t i = 0; i < m_PointLights.size(); i++)
	{
		if (m_PointLights[i]->GetOwner()->IsEnabled())
		{
			pointLights.push_back(m_PointLights[i]);
		}
	}

	return pointLights;
}

std::vector<SpotLight*> Scene::GetEnabledSpotLights() const
{
	std::vector<SpotLight*> spotLights;
	spotLights.reserve(10);
	for (size_t i = 0; i < m_SpotLights.size(); i++)
	{
		if (m_SpotLights[i]->GetOwner()->IsEnabled())
		{
			spotLights.push_back(m_SpotLights[i]);
		}
	}

	return spotLights;
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

GameObject* Scene::FindGameObjectByName(const std::string& name)
{
	auto gameObject = std::find_if(m_GameObjects.begin(), m_GameObjects.end(), [name](GameObject* gameObject) {
		return gameObject->GetName() == name;
	});

	if (gameObject != m_GameObjects.end())
	{
		return *gameObject;
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

GameObject* Scene::FindGameObjectByUUID(const std::string& uuid)
{
	auto gameObject = m_GameObjectsByUUID.find(uuid);
	if (gameObject != m_GameObjectsByUUID.end())
	{
		return gameObject->second;
	}

	return nullptr;
}
