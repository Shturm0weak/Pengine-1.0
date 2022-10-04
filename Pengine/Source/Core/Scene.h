#pragma once

#include "Core.h"
#include "GameObject.h"
#include "../Components/Renderer2D.h"
#include "../Components/Renderer3D.h"
#include "../Components/Rigidbody2D.h"
#include "../Components/BoxCollider2D.h"
#include "../Components/CircleCollider2D.h"
#include "../Components/ParticleEmitter.h"
#include "../Components/PointLight2D.h"
#include "../Components/DirectionalLight.h"
#include "Box2D/include/box2d/b2_world.h"

#include <vector>
#include <unordered_map>
#include <mutex>

namespace Pengine
{

	class PENGINE_API Scene
	{
	private:
		
		std::vector<GameObject*> m_GameObjects;
		std::vector<std::vector<Renderer2D*>> m_Renderer2DLayers;
		std::vector<BoxCollider2D*> m_BoxColliders2D;
		std::vector<CircleCollider2D*> m_CircleColliders2D;
		std::vector<Rigidbody2D*> m_Rigidbody2D;
		std::vector<IRenderer*> m_Renderers3D;
		std::vector<ParticleEmitter*> m_ParticleEmitters;
		std::vector<PointLight2D*> m_PointLights2D;
		std::vector<DirectionalLight*> m_DirectionalLight;
		std::map<Mesh*, std::vector<Renderer3D*>> m_InstancedObjects;

		std::string m_Title = "Scene";
		std::string m_FilePath = "None";
		b2World* m_Box2DWorld = nullptr;

		void Copy(const Scene& scene);
		
		void OnPhysicsUpdate();

		void Render();

		void RenderBoundingBoxes();

		void ShutDown();

		std::vector<GameObject*> SelectGameObject(std::vector<GameObject*> ignoreMask = std::vector<GameObject*>(),
			int maxGameObjects = 1);

		friend GameObject* GameObject::Create(Scene* scene, const std::string& name,
			const Transform& transform, const UUID& uuid);
		friend ICollider2D* BoxCollider2D::IntersectTrigger();
		friend ICollider2D* CircleCollider2D::IntersectTrigger();
		friend void Renderer3D::Render();
		friend IComponent* Renderer2D::Create(GameObject* owner);
		friend IComponent* Renderer3D::Create(GameObject* owner);
		friend IComponent* Rigidbody2D::Create(GameObject* owner);
		friend IComponent* BoxCollider2D::Create(GameObject* owner); 
		friend IComponent* CircleCollider2D::Create(GameObject* owner);
		friend IComponent* ParticleEmitter::Create(GameObject* owner);
		friend IComponent* PointLight2D::Create(GameObject* owner);
		friend IComponent* DirectionalLight::Create(GameObject* owner);
		friend void GameObject::Delete();
		friend void GameObject::DeleteLater(float seconds);
		friend void Renderer2D::Delete();
		friend void Renderer3D::Delete();
		friend void Rigidbody2D::Delete();
		friend void BoxCollider2D::Delete();
		friend void CircleCollider2D::Delete();
		friend void ParticleEmitter::Delete();
		friend void PointLight2D::Delete();
		friend void DirectionalLight::Delete();
		friend void Renderer2D::SetLayer(int layer);
		
		friend class Instancing;
		friend class Application;
		friend class Editor;
		friend class Serializer;
		friend class Raycast2D;
		friend class Batch;
		friend class Renderer;
		friend class Renderer3D;
		friend class EntryPoint;
	public:

		std::mutex m_Mutex;

		Scene(const std::string& title);
		Scene(const Scene& scene);
		Scene(Scene&& scene) noexcept;
		void operator=(const Scene& scene);
		void operator=(Scene&& scene) noexcept;

		b2World& GetBox2DWorld() const { return *m_Box2DWorld; }
		
		std::string GetTitle() const { return m_Title; }
		
		GameObject* CreateGameObject(const std::string& name = "Unnamed",
			const Transform& transform = Transform(), const UUID& uuid = -1);
		
		GameObject* FindGameObject(const std::string& name);
		
		std::vector<GameObject*> FindGameObjects(const std::string& name);
		
		GameObject* FindGameObject(size_t uuid);
		
		void DeleteGameObject(GameObject* gameObject);
		
		void DeleteGameObjectLater(GameObject* gameObject);
		
		const std::vector<GameObject*>& GetGameObjects() const { return m_GameObjects; }
		
		void OnRegisterClients();
		
		void Clear();
	};

}