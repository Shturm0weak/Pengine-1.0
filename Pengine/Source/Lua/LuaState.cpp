#include "LuaState.h"

#include <new>
#include "LuaStateManager.h"
#include "../Core/Logger.h"
#include "../Core/GameObject.h"
#include "../Core/Input.h"
#include "../Core/Camera.h"
#include "../Core/Environment.h"
#include "../Core/Application.h"
#include "../Core/Scene.h"
#include "../Core/EntryPoint.h"
#include "../Core/Visualizer.h"
#include "../Core/Viewport.h"
#include "../Core/Window.h"
#include "../Events/OnMainThreadCallback.h"
#include "../EventSystem/EventSystem.h"
#include "../Components/Script.h"
#include "../Core/MemoryManager.h"

using namespace Pengine;
using namespace luaaa;

bool LuaState::CL(int error)
{
	if (error)
	{
		Logger::Error("LUA", lua_tostring(m_L, -1), m_FilePath.c_str());
		return false;
	}
	return true;
}

bool LuaState::CL(LuaState* l, int error)
{
	if (error)
	{
		Logger::Error("LUA", lua_tostring(l->m_L, -1), l->m_FilePath.c_str());
		return false;
	}
	return true;
}

GameObject* LuaState::LuaGetComponentOwner(void* ptr)
{
	IComponent* iComponent = (IComponent*)ptr;

	return IComponent::IsValid(iComponent) ? iComponent->GetOwner() : nullptr;
}

Transform* LuaState::LuaGetTransform(GameObject* gameObject)
{
	return gameObject ? &gameObject->m_Transform : nullptr;
}

Renderer2D* LuaState::LuaGetRenderer2D(GameObject* gameObject)
{
	return gameObject ? gameObject->m_ComponentManager.GetComponent<Renderer2D>() : nullptr;
}

Renderer2D* LuaState::LuaAddRenderer2D(GameObject* gameObject)
{
	return gameObject ? gameObject->m_ComponentManager.AddComponent<Renderer2D>() : nullptr;
}

Rigidbody2D* LuaState::LuaGetRigidBody2D(GameObject* gameObject)
{
	return gameObject ? gameObject->m_ComponentManager.GetComponent<Rigidbody2D>() : nullptr;
}

Rigidbody2D* LuaState::LuaAddRigidBody2D(GameObject* gameObject)
{
	return gameObject ? gameObject->m_ComponentManager.AddComponent<Rigidbody2D>() : nullptr;
}

ICollider2D* LuaState::LuaGetICollider2D(GameObject* gameObject)
{
	return gameObject ? gameObject->m_ComponentManager.GetComponentSubClass<ICollider2D>() : nullptr;
}

ICollider2D* LuaState::LuaToICollider2D(void* ptr)
{
	ICollider2D* iCollider = (ICollider2D*)ptr;

	return IComponent::IsValid(iCollider) ? iCollider : nullptr;
}

BoxCollider2D* LuaState::LuaGetBoxCollider2D(GameObject* gameObject)
{
	return gameObject ? gameObject->m_ComponentManager.GetComponent<BoxCollider2D>() : nullptr;
}

BoxCollider2D* LuaState::LuaAddBoxCollider2D(GameObject* gameObject)
{
	return gameObject ? gameObject->m_ComponentManager.AddComponent<BoxCollider2D>() : nullptr;
}

Animation2DManager::Animation2D* LuaState::LuaGetAnimation2D(const std::string& name)
{
	return Animation2DManager::GetInstance().Get(name);
}

Animation2DManager::Animation2D* LuaState::LuaCreateAnimation2D(const std::string& filePath)
{
	return Animation2DManager::GetInstance().Create(filePath);
}

Animation2DManager::Animation2D* LuaState::LuaGetAnimation2DFromA2d(Animator2D* a2d, const std::string& name)
{
	return a2d->GetAnimation(name);
}

Animator2D* LuaState::LuaGetAnimator2D(GameObject* gameObject)
{
	return gameObject ? gameObject->m_ComponentManager.GetComponent<Animator2D>() : nullptr;
}

Animator2D* LuaState::LuaAddAnimator2D(GameObject* gameObject)
{
	return gameObject ? gameObject->m_ComponentManager.AddComponent<Animator2D>() : nullptr;
}

PointLight *LuaState::LuaGetPointLight2D(GameObject* gameObject)
{
	return gameObject ? gameObject->m_ComponentManager.GetComponent<PointLight>() : nullptr;;
}

PointLight* LuaState::LuaAddPointLight2D(GameObject* gameObject)
{
	return gameObject ? gameObject->m_ComponentManager.AddComponent<PointLight>() : nullptr;;
}

Texture* LuaState::LuaGetTexture(const std::string& name)
{
	return TextureManager::GetInstance().GetByName(name);
}

Texture* LuaState::LuaCreateTexture(const std::string& filePath)
{
	Texture* returnTexture;
	TextureManager::GetInstance().Create(filePath,
		[&](Texture* texture)
	{
		returnTexture = texture;
	});

	return returnTexture;
}

Shader* LuaState::LuaGetShader(const std::string& name)
{
	return Shader::Get(name);
}

Shader* LuaState::LuaCreateShader(const std::string& filePath)
{
	return Shader::Create(Utils::GetNameFromFilePath(filePath), filePath);
}

CircleCollider2D* LuaState::LuaGetCircleCollider2D(class GameObject* gameObject)
{
	return gameObject ? gameObject->m_ComponentManager.GetComponent<CircleCollider2D>() : nullptr;
}

CircleCollider2D* LuaState::LuaAddCircleCollider2D(class GameObject* gameObject)
{
	return gameObject ? gameObject->m_ComponentManager.AddComponent<CircleCollider2D>() : nullptr;
}

GameObject* LuaState::LuaToGameObject(void* ptr)
{
	GameObject* gameObject = (GameObject*)ptr;

	return GameObject::IsValid(gameObject) ? gameObject : nullptr;
}

GameObject* LuaState::LuaGetOwner(GameObject* gameObject)
{
	return gameObject ? gameObject->GetOwner() : nullptr;
}

void LuaState::OnUpdate(float dt)
{
	if (!m_IsEnabled) return;

	int index = lua_getglobal(m_L, "OnUpdate");
	if (index > 0)
	{
		lua_pushnumber(m_L, Time::GetDeltaTime());
		CL(lua_pcall(m_L, 1, 0, 0));
	}
}

void LuaState::OnStart()
{
	int index = lua_getglobal(m_L, "OnStart");
	if (index > 0)
	{
		CL(lua_pcall(m_L, 0, 0, 0));
	}
}

void LuaState::Initialize(GameObject* owner)
{
	m_GameObject = owner;

	if (m_L) return;

	m_L = luaL_newstate();
	LuaStateManager::GetInstance().m_LuaStatesRaw.push_back(m_L);
	luaL_openlibs(m_L);

	LuaState::RegisterFunction(this);
	CL(luaL_dofile(m_L, m_FilePath.c_str()));

	if (EntryPoint::GetApplication().IsStarted())
	{
		auto callback = [this]() {
			this->OnStart();
		};
		EventSystem::GetInstance().SendEvent(new OnMainThreadCallback(callback, EventType::ONMAINTHREADPROCESS));
	}
}

GameObject* LuaState::GetLuaOwnerGameObject(lua_State* l)
{
	LuaState* luaState = LuaStateManager::GetInstance().GetLuaOwner(l);

	return luaState ? luaState->m_GameObject : nullptr;
}

GameObject* LuaState::LuaCreateGameObject(const std::string& name, lua_State* l)
{
	return GetLuaOwnerGameObject(l)->GetScene()->CreateGameObject(name);
}

void LuaState::RegisterFunction(LuaState* l)
{
	LuaClass<GameObject> gameObject(l->m_L, "GameObject");
	gameObject.ctor("This", &LuaState::GetLuaOwnerGameObject, nullptr);
	gameObject.ctor<const std::string&>("Create", &LuaState::LuaCreateGameObject, nullptr);
	gameObject.ctor<void*>("GetComponentOwner", &LuaState::LuaGetComponentOwner, nullptr);
	gameObject.ctor<GameObject*>("GetOwner", &LuaState::LuaGetOwner, nullptr);
	gameObject.ctor<void*>("To", &LuaState::LuaToGameObject, nullptr);
	gameObject.fun("Delete", &GameObject::DeleteLater);
	gameObject.fun("GetName", &GameObject::GetName);
	gameObject.fun("SetName", &GameObject::SetName);
	gameObject.fun("HasOwner", &GameObject::HasOwner);
	gameObject.fun("IsPrefab", &GameObject::IsPrefab);
	gameObject.fun("ResetWithPrefab", &GameObject::ResetWithPrefab);
	gameObject.fun("UpdatePrefab", &GameObject::UpdatePrefab);
	gameObject.fun("AddChild", &GameObject::AddChild);
	gameObject.fun("RemoveChild", &GameObject::RemoveChild);
	gameObject.fun("Copy", &GameObject::Copy);
	gameObject.fun("IsSerializable", &GameObject::IsSerializable);
	gameObject.fun("SetSerializable", &GameObject::SetSerializable);
	gameObject.fun("IsSelectable", &GameObject::IsSelectable);
	gameObject.fun("SetSelectable", &GameObject::SetSelectable);
	gameObject.fun("IsEnabled", &GameObject::IsEnabled);
	gameObject.fun("SetEnabled", &GameObject::SetEnabled);
	gameObject.fun("GetChilds", &GameObject::GetChilds);

	LuaClass<Transform> transform(l->m_L, "Transform");
	transform.ctor<GameObject*>("Get", &LuaState::LuaGetTransform, nullptr);
	transform.fun("GetPosition", &Transform::GetPosition);
	transform.fun("GetRotation", &Transform::GetRotation);
	transform.fun("GetScale", &Transform::GetScale);
	transform.fun("GetForward", &Transform::GetForward);
	transform.fun("GetBack", &Transform::GetBack);
	transform.fun("GetPositionDelta", &Transform::GetPositionDelta);
	transform.fun("GetPositionMat4", &Transform::GetPositionMat4);
	transform.fun("GetPreviousPosition", &Transform::GetPreviousPosition);
	transform.fun("GetRight", &Transform::GetRight);
	transform.fun("GetRotationMat4", &Transform::GetRotationMat4);
	transform.fun("GetScaleMat4", &Transform::GetScaleMat4);
	transform.fun("GetTransform", &Transform::GetTransform);
	transform.fun("GetUp", &Transform::GetUp);
	transform.fun("Translate", &Transform::Translate);
	transform.fun("Rotate", &Transform::Rotate);
	transform.fun("Scale", &Transform::Scale);
	transform.fun("GetFollorOwner", &Transform::GetFollorOwner);
	transform.fun("SetFollowOwner", &Transform::SetFollowOwner);

	LuaClass<Renderer2D> r2d(l->m_L, "Renderer2D");
	r2d.ctor<GameObject*>("Get", &LuaState::LuaGetRenderer2D, nullptr); 
	r2d.ctor<GameObject*>("Add", &LuaState::LuaAddRenderer2D, nullptr);
	r2d.fun("Copy", &Renderer2D::Copy);
	r2d.fun("Flip", &Renderer2D::Flip);
	r2d.fun("GetLayer", &Renderer2D::GetLayer);
	r2d.fun("SetLayer", &Renderer2D::SetLayer);
	r2d.fun("GetColor", &Renderer2D::GetColor);
	r2d.fun("SetColor", &Renderer2D::SetColor);
	r2d.fun("SetTexture", &Renderer2D::SetTexture);
	r2d.fun("GetTexture", &Renderer2D::GetTexture);
	r2d.fun("OriginalUV", &Renderer2D::OriginalUV);
	r2d.fun("ReversedUV", &Renderer2D::ReversedUV);
	r2d.fun("SetUV", &Renderer2D::SetUV);
	r2d.fun("GetUV", &Renderer2D::GetUV);
	r2d.fun("SetShader", &Renderer2D::SetShader);

	LuaClass<Shader> shader(l->m_L, "Shader");
	shader.ctor<const std::string&>("Get", LuaGetShader, nullptr);
	shader.ctor<const std::string&>("Create", LuaCreateShader, nullptr);
	shader.fun("GetName", &Shader::GetName);
	shader.fun("GetFilePath", &Shader::GetFilePath);

	LuaClass<Texture> texture(l->m_L, "Texture");
	texture.ctor<const std::string&>("Get", LuaGetTexture, nullptr);
	texture.ctor<const std::string&>("Create", LuaCreateTexture, nullptr);
	//texture.fun("GetRendererID", &Texture::GetRendererID);
	texture.fun("GetName", &Texture::GetName);
	texture.fun("GetFilePath", &Texture::GetFilePath);
	texture.fun("GetSize", &Texture::GetSize);

	LuaClass<Rigidbody2D> rb2d(l->m_L, "RigidBody2D");
	rb2d.ctor<GameObject*>("Get", LuaGetRigidBody2D, nullptr);
	rb2d.ctor<GameObject*>("Add", LuaAddRigidBody2D, nullptr);
	rb2d.fun("Copy", &Rigidbody2D::Copy);
	rb2d.fun("IsSleepingAllowed", &Rigidbody2D::IsSleepingAllowed);
	rb2d.fun("SetAllowToSleep", &Rigidbody2D::SetAllowToSleep);
	rb2d.fun("SetStatic", &Rigidbody2D::SetStatic);
	rb2d.fun("IsStatic", &Rigidbody2D::IsStatic);
	rb2d.fun("SetFixedRotation", &Rigidbody2D::SetFixedRotation);
	rb2d.fun("IsFixedRotation", &Rigidbody2D::IsFixedRotation);
	rb2d.fun("ApplyAngularImpulse", &Rigidbody2D::ApplyAngularImpulse);
	rb2d.fun("ApplyForce", &Rigidbody2D::ApplyForce);
	rb2d.fun("ApplyForceToCenter", &Rigidbody2D::ApplyForceToCenter);
	rb2d.fun("ApplyLinearImpulse", &Rigidbody2D::ApplyLinearImpulse);
	rb2d.fun("ApplyLinearImpulseToCenter", &Rigidbody2D::ApplyLinearImpulseToCenter);
	rb2d.fun("ApplyTorque", &Rigidbody2D::ApplyTorque);
	rb2d.fun("SetLinearVelocity", &Rigidbody2D::SetLinearVelocity);

	LuaClass<Animation2DManager::Animation2D> anim(l->m_L, "Animation2D");
	anim.ctor<std::string>("Get", LuaGetAnimation2D, nullptr);
	anim.ctor<std::string>("Create", LuaCreateAnimation2D, nullptr);
	anim.ctor("FromA2d", LuaGetAnimation2DFromA2d, nullptr);
	anim.fun("GetFilePath", &Animation2DManager::Animation2D::GetFilePath);
	anim.fun("GetName", &Animation2DManager::Animation2D::GetName);
	anim.fun("GetUVs", &Animation2DManager::Animation2D::GetUVs);
	anim.fun("GetTextures", &Animation2DManager::Animation2D::GetTextures);

	LuaClass<Animator2D> a2d(l->m_L, "Animator2D");
	a2d.ctor<GameObject*>("Get", LuaGetAnimator2D, nullptr);
	a2d.ctor<GameObject*>("Add", LuaAddAnimator2D, nullptr);
	a2d.fun("Copy", &Animator2D::Copy);
	a2d.fun("GetOriginalUV", &Animator2D::GetOriginalUV);
	a2d.fun("GetReversedUV", &Animator2D::GetReversedUV);
	a2d.fun("Reset", &Animator2D::Reset);
	a2d.fun("RemoveAll", &Animator2D::RemoveAll);
	a2d.fun("SetCurrentAnimation", &Animator2D::SetCurrentAnimation);
	a2d.fun("AddAnimation", &Animator2D::AddAnimation);
	a2d.fun("GetAnimation", &Animator2D::GetAnimation);
	a2d.fun("SetAutoUV", &Animator2D::SetAutoUV);
	a2d.fun("SetSpeed", &Animator2D::SetSpeed);
	a2d.fun("GetSpeed", &Animator2D::GetSpeed);
	a2d.fun("SetPlay", &Animator2D::SetPlay);

	LuaClass<PointLight> pl2d(l->m_L, "PointLight2D");
	pl2d.ctor<GameObject*>("Get", LuaGetPointLight2D, nullptr);
	pl2d.ctor<GameObject*>("Add", LuaAddPointLight2D, nullptr);
	pl2d.fun("Copy", &PointLight::Copy);
	pl2d.fun("SetConstant", &PointLight::SetConstant);
	pl2d.fun("SetLinear", &PointLight::SetLinear);
	pl2d.fun("SetQuadratic", &PointLight::SetQuadratic);
	pl2d.fun("SetColor", &PointLight::SetColor);

	LuaClass<ICollider2D> ic2d(l->m_L, "ICollider2D");
	ic2d.ctor("Get", &LuaState::LuaGetICollider2D, nullptr);
	ic2d.ctor("To", &LuaState::LuaToICollider2D, nullptr);
	ic2d.fun("SetTrigger", &ICollider2D::SetTrigger);

	LuaClass<BoxCollider2D> bc2d(l->m_L, "BoxCollider2D");
	bc2d.ctor<GameObject*>("Get", LuaGetBoxCollider2D, nullptr);
	bc2d.ctor<GameObject*>("Add", LuaAddBoxCollider2D, nullptr);
	bc2d.fun("Copy", &BoxCollider2D::Copy);
	bc2d.fun("SetSize", &BoxCollider2D::SetSize);
	bc2d.fun("GetSize", &BoxCollider2D::GetSize);
	bc2d.fun("SetOffset", &BoxCollider2D::SetOffset);
	bc2d.fun("GetOffset", &BoxCollider2D::GetOffset);
	bc2d.fun("GetPosition", &BoxCollider2D::GetPosition);

	LuaClass<CircleCollider2D> cc2d(l->m_L, "CicrleCollider2D");
	cc2d.ctor<GameObject*>("Get", LuaGetCircleCollider2D, nullptr);
	cc2d.ctor<GameObject*>("Add", LuaAddCircleCollider2D, nullptr);
	cc2d.fun("Copy", &BoxCollider2D::Copy);
	cc2d.fun("SetRadius", &CircleCollider2D::SetRadius);
	cc2d.fun("GetRadius", &CircleCollider2D::GetRadius);

	LuaClass<Raycast2D::Hit2D> hit2D(l->m_L, "Hit2D");
	hit2D.ctor("Create");
	hit2D.fun("GetCollider", &Raycast2D::Hit2D::GetCollider);
	hit2D.fun("GetPosition", &Raycast2D::Hit2D::GetPosition);

	LuaModule window(l->m_L, "Window");
	window.fun("GetSize", []() -> glm::vec2 { return Window::GetInstance().GetSize(); });
	window.fun("SetSize", [](const glm::vec2& size) -> void { Window::GetInstance().SetSize(size); });
	window.fun("SetVsync", [](bool enabled) -> void { Window::GetInstance().SetVSyncEnabled(enabled); });
	window.fun("SetTitle", [](const std::string& title) -> void { Window::GetInstance().SetTitle(title); });

	LuaModule viewport(l->m_L, "Viewport");
	viewport.fun("GetAspect", []() -> float { return Viewport::GetInstance().GetAspect(); });
	viewport.fun("GetDragDelta", []() -> glm::vec2 { return Viewport::GetInstance().GetDragDelta(); });
	viewport.fun("GetMousePosition", []() -> glm::vec2 { return Viewport::GetInstance().GetMousePosition(); });
	viewport.fun("GetMousePositionFromWorldToNormalized", [](const glm::vec2& position) -> glm::vec2 { return Viewport::GetInstance().GetMousePositionFromWorldToNormalized(position); });
	viewport.fun("GetSize", []() -> glm::vec2 { return Viewport::GetInstance().GetSize(); });
	viewport.fun("GetUIDragDelta", []() -> glm::vec2 { return Viewport::GetInstance().GetUIDragDelta(); });
	viewport.fun("GetUIMousePosition", []() -> glm::vec2 { return Viewport::GetInstance().GetUIMousePosition(); });

	LuaModule visualizer(l->m_L, "Visualizer");
	visualizer.fun("DrawQuad", [](const glm::mat4& transform, const glm::vec4& color, Texture* texture) -> void { Visualizer::DrawQuad(transform, color, texture); });
	visualizer.fun("DrawLine", [](const glm::vec3& start, const glm::vec3& end, const glm::vec4& color) -> void { Visualizer::DrawLine(start, end, color); });
	visualizer.fun("DrawCircle", [](const glm::vec2& radius, const glm::mat4& transform, const glm::vec4& color, Texture* texture) -> void { Visualizer::DrawCircle(radius.x, radius.y, transform, color, texture); });

	LuaModule rayCast(l->m_L, "Raycast");
	rayCast.fun("RayCast2D", [](const glm::vec2& from, const glm::vec2& to, Raycast2D::Hit2D* hit,
		std::vector<std::string>& ignoreMaskByTag, lua_State* l) -> void
		{
			Raycast2D::Raycast(GetLuaOwnerGameObject(l)->GetScene(), from, to, *hit, ignoreMaskByTag);
		}
	);

	LuaModule input(l->m_L, "Input");
	input.fun("IsKeyDown", &Input::KeyBoard::IsKeyDown);
	input.fun("IsKeyPressed", &Input::KeyBoard::IsKeyPressed);
	input.fun("IsKeyReleased", &Input::KeyBoard::IsKeyReleased);
	input.fun("IsMouseDown", &Input::Mouse::IsMouseDown);
	input.fun("IsMousePressed", &Input::Mouse::IsMousePressed);
	input.fun("IsMouseReleased", &Input::Mouse::IsMouseReleased);
	input.fun("IsJoyStickDown", &Input::JoyStick::IsButtonDown);
	input.fun("IsJoyStickPressed", &Input::JoyStick::IsButtonPressed);
	input.fun("IsJoyStickReleased", &Input::JoyStick::IsButtonReleased);
	input.fun("GetJoyStickAxis", &Input::JoyStick::GetAxis);

	LuaModule vec2(l->m_L, "vec2");
	vec2.fun("Add", [](const glm::vec2& a, const glm::vec2& b) -> glm::vec2 {return a + b; });
	vec2.fun("Addfv", [](const float& a, const glm::vec2& b) -> glm::vec2 {return a + b; });
	vec2.fun("Addvf", [](const glm::vec2& a, const float& b) -> glm::vec2 {return a + b; });
	vec2.fun("Sub", [](const glm::vec2& a, const glm::vec2& b) -> glm::vec2 {return a - b; });
	vec2.fun("Subfv", [](const float& a, const glm::vec2& b) -> glm::vec2 {return a - b; });
	vec2.fun("Subvf", [](const glm::vec2& a, const float& b) -> glm::vec2 {return a - b; });
	vec2.fun("Mulvf", [](const glm::vec2& a, const float& b) -> glm::vec2 {return a * b; });
	vec2.fun("Mulfv", [](const float& a, const glm::vec2& b) -> glm::vec2 {return a * b; });
	vec2.fun("Cross", [](const glm::vec2& a, const glm::vec2& b) -> glm::vec2 {return glm::cross(glm::vec3(a, 0.0f), glm::vec3(b, 0.0f)); });
	vec2.fun("Dot", [](const glm::vec2& a, const glm::vec2& b) -> float {return glm::dot(a, b); });
	vec2.fun("Normalize", [](const glm::vec2& a) -> glm::vec2 {return glm::normalize(a); });

	LuaModule vec3(l->m_L, "vec3");
	vec3.fun("Add", [](const glm::vec3& a, const glm::vec3& b) -> glm::vec3 {return a + b; });
	vec3.fun("Addfv", [](const float& a, const glm::vec3& b) -> glm::vec3 {return a + b; });
	vec3.fun("Addvf", [](const glm::vec3& a, const float& b) -> glm::vec3 {return a + b; });
	vec3.fun("Sub", [](const glm::vec3& a, const glm::vec3& b) -> glm::vec3 {return a - b; });
	vec3.fun("Subfv", [](const float& a, const glm::vec3& b) -> glm::vec3 {return a - b; });
	vec3.fun("Subvf", [](const glm::vec3& a, const float& b) -> glm::vec3 {return a - b; });
	vec3.fun("Mulvf", [](const glm::vec3& a, const float& b) -> glm::vec3 {return a * b; });
	vec3.fun("Mulfv", [](const float& a, const glm::vec3& b) -> glm::vec3 {return a * b; });
	vec3.fun("Cross", [](const glm::vec3& a, const glm::vec3& b) -> glm::vec3 {return glm::cross(a, b); });
	vec3.fun("Dot", [](const glm::vec3& a, const glm::vec3& b) -> float {return glm::dot(a, b); });
	vec3.fun("Normalize", [](const glm::vec3& a) -> glm::vec3 {return glm::normalize(a); });

	LuaModule vec4(l->m_L, "vec4");
	vec4.fun("Add", [](const glm::vec4& a, const glm::vec4& b) -> glm::vec4 {return a + b; });
	vec4.fun("Addfv", [](const float& a, const glm::vec4& b) -> glm::vec4 {return a + b; });
	vec4.fun("Addvf", [](const glm::vec4& a, const float& b) -> glm::vec4 {return a + b; });
	vec4.fun("Sub", [](const glm::vec4& a, const glm::vec4& b) -> glm::vec4 {return a - b; });
	vec4.fun("Subfv", [](const float& a, const glm::vec4& b) -> glm::vec4 {return a - b; });
	vec4.fun("Subvf", [](const glm::vec4& a, const float& b) -> glm::vec4 {return a - b; });
	vec4.fun("Mulvf", [](const glm::vec4& a, const float& b) -> glm::vec4 {return a * b; });
	vec4.fun("Mulfv", [](const float& a, const glm::vec4& b) -> glm::vec4 {return a * b; });
	vec4.fun("Cross", [](const glm::vec4& a, const glm::vec4& b) -> glm::vec3 {return glm::cross((glm::vec3)a, (glm::vec3)b); });
	vec4.fun("Dot", [](const glm::vec4& a, const glm::vec4& b) -> float {return glm::dot((glm::vec3)a, (glm::vec3)b); });
	vec4.fun("Normalize", [](const glm::vec4& a) -> glm::vec4 {return glm::normalize(a); });

	LuaModule mat3(l->m_L, "mat3");
	mat3.fun("Add", [](const glm::mat3& a, const glm::mat3& b) -> glm::mat3 {return a + b; });
	mat3.fun("Sub", [](const glm::mat3& a, const glm::mat3& b) -> glm::mat3 {return a - b; });
	mat3.fun("Mul", [](const glm::mat3& a, const glm::mat3& b) -> glm::mat3 {return a * b; });
	mat3.fun("Mulmf", [](const glm::mat3& a, const float& b) -> glm::mat3 {return a * b; });
	mat3.fun("Mulfm", [](const float& a, const glm::mat3& b) -> glm::mat3 {return a * b; });
	mat3.fun("Inverse", [](const glm::mat3& a) -> glm::mat3 {return glm::inverse(a); });
	mat3.fun("Print", [](const glm::mat3& a) -> void {return Utils::Print(a); });

	LuaModule mat4(l->m_L, "mat4");
	mat4.fun("Add", [](const glm::mat4& a, const glm::mat4& b) -> glm::mat4 {return a + b; });
	mat4.fun("Sub", [](const glm::mat4& a, const glm::mat4& b) -> glm::mat4 {return a - b; });
	mat4.fun("Mul", [](const glm::mat4& a, const glm::mat4& b) -> glm::mat4 {return a * b; });
	mat4.fun("Mulmf", [](const glm::mat4& a, const float& b) -> glm::mat4 {return a * b; });
	mat4.fun("Mulfm", [](const float& a, const glm::mat4& b) -> glm::mat4 {return a * b; });
	mat4.fun("Inverse", [](const glm::mat4& a) -> glm::mat4 {return glm::inverse(a); });
	mat4.fun("Print", [](const glm::mat4& a) -> void {return Utils::Print(a); });

	LuaModule global(l->m_L);
	global.fun("GetType", [](void* ptr) -> std::string 
		{
			IComponent* iComponent = (IComponent*)ptr;

			return IComponent::IsValid(iComponent) ? ((IComponent*)iComponent)->GetType() : "The component is not valid"; 
		}
	);
	global.fun("AttachScript", [](GameObject* gameObject, const std::string& filePath) -> void
		{
			if (gameObject)
			{
				Script* script = gameObject->m_ComponentManager.AddComponent<Script>();
				script->Assign(filePath);
			}
		}
	);
}
