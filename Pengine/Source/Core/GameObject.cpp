#include "GameObject.h"

#include "MemoryManager.h"
#include "Logger.h"
#include "Scene.h"
#include "Utils.h"
#include "Serializer.h"
#include "../Events/OnMainThreadCallback.h"
#include "../EventSystem/EventSystem.h"

#include <unordered_map>

using namespace Pengine;

void GameObject::Copy(const GameObject& gameObject)
{
	m_Name = gameObject.m_Name;
	m_Transform = gameObject.m_Transform;
	m_ComponentManager = gameObject.m_ComponentManager;
	m_IsSerializable = gameObject.m_IsSerializable;
	m_IsEnabled = gameObject.m_IsEnabled;
	m_IsSelectable = gameObject.m_IsSelectable;
	m_PrefabFilePath = gameObject.m_PrefabFilePath;

	if (gameObject.GetOwner())
	{
		gameObject.GetOwner()->AddChild(this);
	}

	while (m_Childs.size() > 0)
	{
		m_Childs.back()->Delete();
	}

	if (m_Owner)
	{
		m_Owner->RemoveChild(this);
	}

	std::vector<GameObject*> childs = gameObject.GetChilds();
	for (GameObject* child : childs)
	{
		GameObject* newChild = m_Scene->CreateGameObject(child->m_Name);
		*newChild = *child;
		AddChild(newChild);
	}
}

void GameObject::Move(GameObject&& gameObject) noexcept
{
	m_Name = std::move(gameObject.m_Name);
	m_Transform = std::move(gameObject.m_Transform);
}

GameObject::GameObject(GameObject&& gameObject) noexcept 
	: m_Name(std::move(gameObject.m_Name))
	, m_Transform(std::move(gameObject.m_Transform))
{
}

GameObject::GameObject(const GameObject& gameObject)
{
	Copy(gameObject);
}

GameObject::GameObject(const std::string& name, const Transform& transform)
{
	m_Name = name;
	m_Transform = transform;
}

void GameObject::operator=(const GameObject& gameObject)
{
	Copy(gameObject);
}

GameObject::~GameObject()
{
}

void GameObject::operator=(GameObject&& gameObject) noexcept
{
	Move(std::move(gameObject));
}

GameObject* GameObject::Create(Scene* scene, const std::string& name, const Transform& transform, const UUID& uuid)
{
	assert(scene != nullptr);

	GameObject* gameObject = MemoryManager::GetInstance().Allocate<GameObject>();
	gameObject->m_Name = name;
	gameObject->m_Transform = transform;
	gameObject->m_Scene = scene;
	gameObject->m_Scene->m_GameObjects.push_back(gameObject);
	gameObject->m_Transform.m_Owner = gameObject;
	gameObject->m_UUID = uuid.operator size_t();

#ifdef _DEBUG
	Logger::Log("has been created!", "GameObject", gameObject->m_Name.c_str(), RESET);
#endif

	return gameObject;
}

void GameObject::Delete()
{
	while (m_Childs.size() > 0)
	{
		m_Childs.back()->Delete();
	}

	if (m_Owner)
	{
		m_Owner->RemoveChild(this);
	}

	Utils::Erase<GameObject>(m_Scene->m_GameObjects, this);

	m_ComponentManager.Clear();
	m_UUID.Clear();
	MemoryManager::GetInstance().FreeMemory<GameObject>(static_cast<IAllocator*>(this));

#ifdef _DEBUG
	Logger::Log("has been deleted!", "GameObject", m_Name.c_str(), RESET);
#endif
}

void GameObject::ForChilds(std::function<void(GameObject& child)> forChilds)
{
	for (GameObject* child : m_Childs)
	{
		forChilds(*child);
	}
}

void GameObject::DeleteLater()
{
	std::function<void()> callback = std::function<void()>([this] {
		this->Delete();
	});
	EventSystem::GetInstance().SendEvent(new OnMainThreadCallback(callback, EventType::ONMAINTHREADPROCESS));
}

void GameObject::AddChild(GameObject* child)
{
	if (this == child || child == this->GetOwner()) return;
	if (child->GetOwner() != nullptr)
	{
		child->GetOwner()->RemoveChild(child);
	}
	if (std::find(m_Childs.begin(), m_Childs.end(), child) == m_Childs.end())
	{
		m_Childs.push_back(child);
	}

	child->m_Owner = this;
}

void GameObject::RemoveChild(GameObject* child)
{
	if (Utils::Erase<GameObject>(m_Childs, child))
	{
		child->m_Owner = nullptr;
	}
}

void GameObject::ResetWithPrefab()
{
	auto callback = [this]() {
		GameObject* prefab = Serializer::DeserializePrefab(this->m_PrefabFilePath);
		if (prefab)
		{
			Transform tempTransform(this->m_Transform);
			this->Copy(*prefab);
			this->m_Transform.Copy(tempTransform);
			prefab->DeleteLater();
		}
	};
	EventSystem::GetInstance().SendEvent(new OnMainThreadCallback(callback, EventType::ONMAINTHREADPROCESS));
}

void GameObject::UpdatePrefab()
{
	Serializer::SerializePrefab(Utils::GetDirectoryFromFilePath(m_PrefabFilePath), *this);

}
