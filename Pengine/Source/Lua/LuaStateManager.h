#pragma once

#include "../Core/Core.h"
#include "LuaState.h"

#include <unordered_map>

namespace Pengine
{

	class PENGINE_API LuaStateManager
	{
	private:

		std::vector<class LuaState*> m_LuaStates;
		std::vector<class lua_State*> m_LuaStatesRaw;

		LuaStateManager() = default;
		LuaStateManager(const LuaStateManager&) = delete;
		LuaStateManager& operator=(const LuaStateManager&) { return *this; }
		~LuaStateManager() = default;

		friend class Editor;
		friend class Application;
		friend class LuaState;
	public:

		static LuaStateManager& GetInstance();

		class LuaState* GetLuaOwner(class lua_State* l);
		
		class LuaState* Create(const std::string& filePath);
		
		void Remove(class LuaState* luaState);
		
		void ShutDown();
	};

}
