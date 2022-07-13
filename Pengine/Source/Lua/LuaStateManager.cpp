#include "LuaStateManager.h"

#include "../Core/Utils.h"
#include "../Core/MemoryManager.h"
#include "../Core/Logger.h"

using namespace Pengine;

LuaStateManager& LuaStateManager::GetInstance()
{
    static LuaStateManager luaStateManager;
    return luaStateManager;
}

LuaState* LuaStateManager::GetLuaOwner(lua_State* l)
{
    for (auto& state : m_LuaStates)
    {
        if (state->m_L == l)
        {
            return state;
        }
    }

    return nullptr;
}

LuaState* LuaStateManager::Create(const std::string& filePath)
{
    LuaState* luaState = new LuaState(filePath);
    m_LuaStates.push_back(luaState);

    return luaState;
}

void LuaStateManager::Remove(LuaState* luaState)
{
    if (!luaState) return;

    Utils::Erase<LuaState>(m_LuaStates, luaState);

    delete luaState;
}

void LuaStateManager::ShutDown()
{
    for (auto& state : m_LuaStatesRaw)
    {
        if (state)
        {
            lua_close(state);
        }
    }

    m_LuaStatesRaw.clear();
}