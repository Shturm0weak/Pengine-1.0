#pragma once

#include "Core.h"
#include "ComponentManager.h"
#include "Allocator.h"
#include "UUID.h"

#include <map>
#include <vector>

namespace Pengine
{

	class Scene;

	class PENGINE_API GameObject : public IAllocator
	{
	private:

		size_t m_CheckTypeID = 0; // Used to see whether a void ptr from lua is castable.

		std::vector<GameObject*> m_Childs;
		std::string m_PrefabFilePath = "";
		std::string m_Name = "Unnamed";

		GameObject* m_Owner = nullptr;
		Scene* m_Scene = nullptr;

		UUID m_UUID = UUID();
		
		bool m_IsSerializable = true;

		void Move(GameObject&& gameObject) noexcept;

		friend class Scene;
		friend class GameObjectManager;
		friend class Editor;
		friend class Serializer;
		friend class LuaState;
	public:

		bool m_IsEnabled = true;
		bool m_IsSelectable = true;
		
		GameObject(GameObject&& gameObject) noexcept;
		GameObject(const GameObject& gameObject);
		GameObject(const std::string& name = "Unnamed", const Transform& transform = Transform());
		~GameObject();
		void operator=(const GameObject& gameObject);
		void operator=(GameObject&& gameObject) noexcept;

		Transform m_Transform;
		ComponentManager m_ComponentManager = ComponentManager(this);

		static bool IsValid(GameObject* gameObject) { return gameObject && gameObject->m_CheckTypeID == 0; }
		
		static GameObject* Create(Scene* scene, const std::string& name = "Unnamed",
			const Transform& transform = Transform(), const UUID& uuid = -1);

		void Copy(const GameObject& gameObject);
		
		bool IsSerializable() const { return m_IsSerializable; }
		
		void SetSerializable(bool isSerializable) { m_IsSerializable = isSerializable; }
		
		bool IsSelectable() const { return m_IsSelectable; }
		
		void SetSelectable(bool isSelectable) { m_IsSelectable = isSelectable; }
		
		bool IsEnabled() const { return m_IsEnabled; }
		
		void SetEnabled(bool isEnabled) { m_IsEnabled = isEnabled; }

		void Delete();
		
		void DeleteLater();
		
		bool IsPrefab() const { return !m_PrefabFilePath.empty(); }
		
		std::string GetName() const { return m_Name; }
		
		UUID GetUUID() const { return m_UUID; }
		
		void SetName(const std::string& name) { m_Name = name; }
		
		const std::vector<GameObject*>& GetChilds() const { return m_Childs; }
		
		void ForChilds(std::function<void(GameObject& child)> forChilds);
		
		void AddChild(GameObject* child);
		
		void RemoveChild(GameObject* child);
		
		bool HasOwner() const { return m_Owner != nullptr; }
		
		GameObject* GetOwner() const { return m_Owner; }
		
		Scene* GetScene() const { return m_Scene; }
		
		void ResetWithPrefab();
		
		void UpdatePrefab();
	};

}