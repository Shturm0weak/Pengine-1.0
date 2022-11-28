#pragma once

#include "Core/Core.h"
#include "Core/Application.h"
#include "BulletCommon/btBulletDynamicsCommon.h"

#include <vector>

using namespace Pengine;

class BulletPhysics : public Application
{
private:

	class GameObject* interesectedGameObject = nullptr;
public:

	BulletPhysics(const std::string& title = "BulletPhysics")
		: Application(title)
	{}

	virtual void OnStart() override;

	virtual void OnPostStart() override {};

	virtual void OnUpdate() override;

	virtual void OnGuiRender() override {};

	virtual void OnImGuiRender() override {};

	virtual void OnClose() override;
};