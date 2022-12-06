#pragma once

#include "Core.h"
#include "GameObject.h"
#include "../Components/Renderer2D.h"
#include "../Components/Renderer3D.h"
#include "../Components/Rigidbody2D.h"
#include "../Components/Rigidbody3D.h"
#include "../Components/BoxCollider2D.h"
#include "../Components/BoxCollider3D.h"
#include "../Components/CircleCollider2D.h"
#include "../Components/ParticleEmitter.h"
#include "../Components/PointLight.h"
#include "../Components/SphereCollider3D.h"
#include "../Components/DirectionalLight.h"
#include "../Components/SpotLight.h"
#include "Box2D/include/box2d/b2_world.h"
#include "BulletPhysicsWorld.h"

#include <vector>
#include <unordered_map>
#include <mutex>

namespace Pengine
{

	class PENGINE_API Scene
	{
	private:
		
		std::unordered_map<Mesh*, std::vector<Renderer3D*>> m_OpaqueByMesh;
		std::unordered_map<Mesh*, std::vector<Renderer3D*>> m_ShadowsByMesh;
		std::unordered_map<std::string, GameObject*> m_GameObjectsByUUID;
		std::vector<GameObject*> m_GameObjects;
		std::vector<std::vector<Renderer2D*>> m_Renderer2DLayers;
		std::vector<BoxCollider2D*> m_BoxColliders2D;
		std::vector<BoxCollider3D*> m_BoxColliders3D;
		std::vector<CircleCollider2D*> m_CircleColliders2D;
		std::vector<SphereCollider3D*> m_SphereColliders3D;
		std::vector<Rigidbody2D*> m_Rigidbody2D;
		std::vector<Rigidbody3D*> m_Rigidbody3D;
		std::vector<ParticleEmitter*> m_ParticleEmitters;
		std::vector<PointLight*> m_PointLights;
		std::vector<SpotLight*> m_SpotLights;
		std::vector<DirectionalLight*> m_DirectionalLights;
		
		std::vector<Renderer3D*> m_TransparentByDistance;
		std::vector<Renderer3D*> m_Renderers3D;

		std::string m_Title = "Scene";
		std::string m_FilePath = "None";
		b2World* m_Box2DWorld = nullptr;
		BulletPhysicsWorld m_BulletPhysicsWorld;

		void Copy(const Scene& scene);
		
		void OnPhysicsUpdate();

		void Render();

		void PrepareVisualizer();

		void PrepareToRender();

		void SortTransparent();

		void SortPointLights();

		void RenderBoundingBoxes();

		void ShutDown();

		void CreatePhysics();

		void DestroyPhysics();

		std::vector<PointLight*> GetEnabledPointLights() const;

		std::vector<SpotLight*> GetEnabledSpotLights() const;

		std::vector<GameObject*> SelectGameObject(std::vector<GameObject*> ignoreMask = std::vector<GameObject*>(),
			int maxGameObjects = 1);

		
		friend class GameObject;
		friend class ParticleEmitter;
		friend class DirectionalLight;
		friend class SpotLight;
		friend class PointLight;
		friend class BoxCollider3D;
		friend class BoxCollider2D;
		friend class CircleCollider2D;
		friend class SphereCollider3D;
		friend class Rigidbody3D;
		friend class Rigidbody2D;
		friend class Instancing;
		friend class Application;
		friend class Editor;
		friend class Serializer;
		friend class Raycast2D;
		friend class Batch;
		friend class Renderer;
		friend class Renderer3D;
		friend class Renderer2D;
		friend class EntryPoint;
	public:

		Scene(const std::string& title);
		Scene(const Scene& scene);
		Scene(Scene&& scene) noexcept;
		void operator=(const Scene& scene);
		void operator=(Scene&& scene) noexcept;

		b2World& GetBox2DWorld() const { return *m_Box2DWorld; }

		BulletPhysicsWorld& GetBulletPhysicsWorld() { return m_BulletPhysicsWorld; }
		
		std::string GetTitle() const { return m_Title; }
		
		GameObject* CreateGameObject(const std::string& name = "Unnamed",
			const Transform& transform = Transform(), const UUID& uuid = UUID());
		
		GameObject* FindGameObjectByName(const std::string& name);
		
		std::vector<GameObject*> FindGameObjects(const std::string& name);
		
		GameObject* FindGameObjectByUUID(const std::string& uuid);
		
		void DeleteGameObject(GameObject* gameObject);
		
		void DeleteGameObjectLater(GameObject* gameObject);
		
		const std::vector<GameObject*>& GetGameObjects() const { return m_GameObjects; }
		
		std::vector<BoxCollider3D*> GetBoxColliders3D() const { return m_BoxColliders3D; }

		void OnRegisterClients();
		
		void Clear();
	};

}