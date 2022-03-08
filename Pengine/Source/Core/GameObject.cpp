#include "GameObject.h"

#include "MemoryManager.h"
#include "Logger.h"
#include "Scene.h"
#include "Utils.h"
#include "../Events/OnMainThreadCallback.h"
#include "../EventSystem/EventSystem.h"

#include <unordered_map>

using namespace Pengine;

void GameObject::Copy(const GameObject& gameObject)
{
	m_Name = gameObject.m_Name;
	m_Transform = gameObject.m_Transform;
	m_ComponentManager = gameObject.m_ComponentManager;

	if (gameObject.GetOwner())
	{
		gameObject.GetOwner()->AddChild(this);
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

void GameObject::AddChild(GameObject* child)
{
	if (this == child || child == this->GetOwner() || child->GetOwner() == this) return;
	if (child->GetOwner() != nullptr)
	{
		child->GetOwner()->RemoveChild(child);
	}
	m_Childs.push_back(child);
	child->m_Owner = this;
}

void GameObject::RemoveChild(GameObject* child)
{
	if (Utils::Erase<GameObject>(m_Childs, child))
	{
		child->m_Owner = nullptr;
	}
}