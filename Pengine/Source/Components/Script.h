#pragma once

#include "../Core/Core.h"
#include "../Core/Component.h"
#include "../Lua/LuaState.h"

namespace Pengine
{

	class Script : public IComponent
	{
	private:

		std::vector<LuaState*> m_LStates;

		friend class Editor;
		friend class Serializer;
	public:

		Script() = default;
		~Script();
		
		static IComponent* Create(GameObject* owner);
		
		virtual IComponent* CreateCopy(GameObject* newOwner) override;
		
		virtual IComponent* New(GameObject* owner) override;

		virtual void Copy(const IComponent& component) override;

		LuaState* Get(const std::string& filePath);
		
		void Remove(const std::string& filePath);
		
		void Assign(const std::string& filePath);
		
		void Initialize();
	};

}