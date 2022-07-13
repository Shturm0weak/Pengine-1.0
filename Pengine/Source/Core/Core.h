#pragma once

#define MAX_PREALLOCATED_INSTANCES 1000

#ifdef PENGINE_ENGINE
#define PENGINE_API __declspec(dllexport)
#else
#define PENGINE_API __declspec(dllimport)
#endif

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/quaternion.hpp"

#include <ImGui/imgui.h>
#include <ImGui/examples/imgui_impl_glfw.h>
#include <ImGui/examples/imgui_impl_opengl3.h>

#include "Time.h"

#include <string>

// Used for RTTR.
template<typename T>
inline void ReflectionAddComponent(void* componentManager)
{
	static_cast<Pengine::ComponentManager*>(componentManager)->AddComponent<T>();
}