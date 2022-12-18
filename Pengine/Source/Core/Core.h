#pragma once

#define MAX_PREALLOCATED_INSTANCES 1000

#ifdef PENGINE_ENGINE
#define PENGINE_API __declspec(dllexport)
#else
#define PENGINE_API __declspec(dllimport)
#endif

//#define STANDALONE
#undef STANDALONE

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/quaternion.hpp"

#include "../Vendor/ImGui/imgui_internal.h"
#include "../Vendor/ImGui/backends/imgui_impl_glfw.h"
#include "../Vendor/ImGui/backends/imgui_impl_opengl3.h"

#include "../Enums/Colors.h"
#include "Time.h"

#include <string>

constexpr char* none = "None";

// Used for RTTR.
template<typename T>
inline void ReflectionAddComponent(void* componentManager)
{
	static_cast<Pengine::ComponentManager*>(componentManager)->AddComponent<T>();
}

template<typename T>
std::string GetTypeName()
{
	std::string typeName = std::string(typeid(T).name());
	if (typeName.find(std::string("class")) == 0)
	{
		return typeName.substr(sizeof("class"));
	}
	else if (typeName.find(std::string("struct")) == 0)
	{
		return typeName.substr(sizeof("struct"));
	}
	else
	{
		return typeName;
	}
}