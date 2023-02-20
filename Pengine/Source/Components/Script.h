#pragma once

#include "../Core/Core.h"
#include "../Core/Component.h"
#include "../Lua/LuaState.h"

namespace Pengine
{

	/**
	 * @Deprectated. No longer supported.
	 */
	class Script : public IComponent
	{
	private:

		std::vector<LuaState*> m_LStates;

		friend class Editor;
		friend class Serializer;
	protected:

		virtual void Copy(const IComponent& component) override;

		virtual IComponent* New(GameObject* owner) override;

		virtual IComponent* CreateCopy(GameObject* newOwner) override;
	public:

		static IComponent* Create(GameObject* owner);

		Script() = default;

		~Script();

		LuaState* Get(const std::string& filePath);
		
		void Remove(const std::string& filePath);
		
		void Assign(const std::string& filePath);
		
		void Initialize();
	};

}