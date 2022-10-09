#pragma once

#include "../Core/Core.h"
#include "luaaa.hpp"
#include "../Core/Raycast2D.h"
#include "../Components/Animator2D.h"

#include <vector>

namespace luaaa
{

	template<> struct LuaStack<void*>
	{
		inline static void* get(lua_State* l, int idx)
		{
			if (lua_isuserdata(l, idx))
			{
				void* ptr = reinterpret_cast<void*>(*(static_cast<size_t*>(lua_touserdata(l, idx))));

				return (size_t)ptr == 0xcdcdcdcdcdcdcd01 ? lua_touserdata(l, idx) : ptr;
			}
			else
			{
				return nullptr;
			}
		}

		inline static void put(lua_State* l, void* ptr)
		{
			LuaStack<decltype(ptr)>::put(l, ptr);
		}
	};

	template<> struct LuaStack<glm::vec2>
	{
		inline static glm::vec2 get(lua_State* l, int idx)
		{
			auto dict = LuaStack<std::map<std::string, float>>::get(l, idx);
			return glm::vec2(dict.find("x")->second, dict.find("y")->second);
		}

		inline static void put(lua_State* l, const glm::vec2& v)
		{
			std::map<std::string, float> dict;
			dict["x"] = v.x;
			dict["y"] = v.y;
			LuaStack<decltype(dict)>::put(l, dict);
		}
	};

	template<> struct LuaStack<glm::vec3>
	{
		inline static glm::vec3 get(lua_State* l, int idx)
		{
			auto dict = LuaStack<std::map<std::string, float>>::get(l, idx);
			return glm::vec3(dict.find("x")->second, dict.find("y")->second, dict.find("z")->second);
		}

		inline static void put(lua_State* l, const glm::vec3& v)
		{
			std::map<std::string, float> dict;
			dict["x"] = v.x;
			dict["y"] = v.y;
			dict["z"] = v.z;
			LuaStack<decltype(dict)>::put(l, dict);
		}
	};

	template<> struct LuaStack<glm::vec4>
	{
		inline static glm::vec4 get(lua_State* l, int idx)
		{
			auto dict = LuaStack<std::map<std::string, float>>::get(l, idx);
			return glm::vec4(dict.find("x")->second, dict.find("y")->second, dict.find("z")->second, dict.find("w")->second);
		}

		inline static void put(lua_State* l, const glm::vec4& v)
		{
			std::map<std::string, float> dict;
			dict["x"] = v.x;
			dict["y"] = v.y;
			dict["z"] = v.z;
			dict["w"] = v.w;
			LuaStack<decltype(dict)>::put(l, dict);
		}
	};

	template<> struct LuaStack<glm::mat3>
	{
		inline static glm::mat3 get(lua_State* l, int idx)
		{
			auto dict = LuaStack<std::map<std::string, float>>::get(l, idx);
			glm::mat3 mat = glm::mat3(dict.find("i0")->second);
			for (size_t i = 0; i < 3; i++)
			{
				for (size_t j = 0; j < 3; j++)
				{
					auto matIter = dict.find("i" + std::to_string(i * 3 + j));
					if (matIter != dict.end())
					{
						mat[j][i] = matIter->second;
					}
				}
			}

			return mat;
		}

		inline static void put(lua_State* l, const glm::mat3& mat)
		{
			std::map<std::string, float> dict;
			dict["i0"] = mat[0][0];
			dict["i1"] = mat[1][0];
			dict["i2"] = mat[2][0];
			dict["i3"] = mat[0][1];
			dict["i4"] = mat[1][1];
			dict["i5"] = mat[2][1];
			dict["i6"] = mat[0][2];
			dict["i7"] = mat[1][2];
			dict["i8"] = mat[2][2];
			LuaStack<decltype(dict)>::put(l, dict);
		}
	};


	template<> struct LuaStack<glm::mat4>
	{
		inline static glm::mat4 get(lua_State* l, int idx)
		{
			auto dict = LuaStack<std::map<std::string, float>>::get(l, idx);
			glm::mat4 mat = glm::mat4(dict.find("i0")->second);
			for (size_t i = 0; i < 4; i++)
			{
				for (size_t j = 0; j < 4; j++)
				{
					auto matIter = dict.find("i" + std::to_string(i * 4 + j));
					if (matIter != dict.end())
					{
						mat[j][i] = matIter->second;
					}
				}
			}

			return mat;
		}

		inline static void put(lua_State* l, const glm::mat4& mat)
		{
			std::map<std::string, float> dict;
			dict["i0"] = mat[0][0];
			dict["i1"] = mat[1][0];
			dict["i2"] = mat[2][0];
			dict["i3"] = mat[3][0];
			dict["i4"] = mat[0][1];
			dict["i5"] = mat[1][1];
			dict["i6"] = mat[2][1];
			dict["i7"] = mat[3][1];
			dict["i8"] = mat[0][2];
			dict["i9"] = mat[1][2];
			dict["i10"] = mat[2][2];
			dict["i11"] = mat[3][2];
			dict["i12"] = mat[0][3];
			dict["i13"] = mat[1][3];
			dict["i14"] = mat[2][3];
			dict["i15"] = mat[3][3];
			LuaStack<decltype(dict)>::put(l, dict);
		}
	};
}

namespace Pengine
{

	class PENGINE_API LuaState
	{
	public:

		lua_State* m_L = nullptr;
		std::string m_FilePath;
		bool m_IsEnabled = true;

		// Check Lua.
		bool CL(int error); 
		static bool CL(LuaState* l, int error);

		static class Transform* LuaGetTransform(class GameObject* gameObject);

		static class Renderer2D* LuaGetRenderer2D(class GameObject* gameObject);
		static class Renderer2D* LuaAddRenderer2D(class GameObject* gameObject);

		static class Rigidbody2D* LuaGetRigidBody2D(class GameObject* gameObject);
		static class Rigidbody2D* LuaAddRigidBody2D(class GameObject* gameObject);

		static class ICollider2D* LuaGetICollider2D(class GameObject* gameObject);
		static class ICollider2D* LuaToICollider2D(void* ptr);

		static class BoxCollider2D* LuaGetBoxCollider2D(class GameObject* gameObject);
		static class BoxCollider2D* LuaAddBoxCollider2D(class GameObject* gameObject);

		static Animation2DManager::Animation2D* LuaGetAnimation2D(const std::string& name);
		static Animation2DManager::Animation2D* LuaCreateAnimation2D(const std::string& filePath);
		static Animation2DManager::Animation2D* LuaGetAnimation2DFromA2d(Animator2D* a2d, const std::string& name);

		static class Animator2D* LuaGetAnimator2D(class GameObject* gameObject);
		static class Animator2D* LuaAddAnimator2D(class GameObject* gameObject);

		static class PointLight* LuaGetPointLight2D(class GameObject* gameObject);
		static class PointLight* LuaAddPointLight2D(class GameObject* gameObject);

		static class Texture* LuaGetTexture(const std::string& name);
		static class Texture* LuaCreateTexture(const std::string& filePath);

		static class Shader* LuaGetShader(const std::string& name);
		static class Shader* LuaCreateShader(const std::string& filePath);

		static class CircleCollider2D* LuaGetCircleCollider2D(class GameObject* gameObject);
		static class CircleCollider2D* LuaAddCircleCollider2D(class GameObject* gameObject);

		static class GameObject* LuaToGameObject(void* ptr);
		static class GameObject* LuaGetOwner(class GameObject* gameObject);
		static class GameObject* LuaGetComponentOwner(void* ptr);
	public:

		class GameObject* m_GameObject = nullptr;

		LuaState(const std::string& filePath) { m_FilePath = filePath; };
		~LuaState() = default;
		
		static class GameObject* GetLuaOwnerGameObject(lua_State* l);
		static class GameObject* LuaCreateGameObject(const std::string& name, lua_State* l);
		static void RegisterFunction(LuaState* l);
		
		void OnUpdate(float dt);
		
		void OnStart();

		void Initialize(class GameObject* owner);

		friend class Editor;
	};

}