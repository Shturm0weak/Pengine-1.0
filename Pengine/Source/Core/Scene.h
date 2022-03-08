#pragma once

#include "Core.h"
#include "GameObject.h"
#include "../Components/Renderer2D.h"
#include "../Components/Renderer3D.h"
#include "../Components/Rigidbody2D.h"
#include "Box2D/include/box2d/b2_world.h"

#include <vector>
#include <unordered_map>

namespace Pengine
{

	class PENGINE_API Scene
	{
	private:
		
		std::vector<GameObject*> m_GameObjects;
		std::unordered_map<UUID, IRenderer*> m_Renderers2D;
		std::unordered_map<UUID, IRenderer*> m_Renderers3D;
		std::unordered_map<UUID, Rigidbody2D*> m_Rigidbody2D;

		std::vector<std::vector<Renderer2D*>> m_Renderer2DLayers;

		std::string m_Title;
		b2World* m_Box2DWorld = nullptr;
		void Copy(const Scene& scene);

		void OnPhysicsStart();
		void OnPhysicsUpdate();
		void OnPhysicsClose();

		std::vector<GameObject*> SelectGameObject(std::vector<GameObject*> ignoreMask = std::vector<GameObject*>(),
			int maxGameObjects = 1);

		friend GameObject* GameObject::Create(Scene* scene, const std::string& name,
			const Transform& transform, const UUID& uuid);
		friend IComponent* Renderer2D::Create(GameObject* owner);
		friend IComponent* Renderer3D::Create(GameObject* owner);
		friend IComponent* Rigidbody2D::Create(GameObject* owner);
		friend void GameObject::Delete();
		friend void Renderer2D::Delete();
		friend void Renderer3D::Delete();
		friend void Rigidbody2D::Delete();
		friend void Renderer2D::SetLayer(int layer);

		friend class Application;
		friend class Editor;
		friend class Serializer;
	public:

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
		GameObject* FindGameObject(size_t uuid);
		void DeleteGameObject(GameObject* gameObject);
		const std::vector<GameObject*>& GetGameObjects() const { return m_GameObjects; }
		void Render();
		void ShutDown();
		void Clear();
	};

}