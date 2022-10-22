#pragma once

#include "Core/Core.h"
#include "Core/Application.h"
#include "Core/Camera.h"
#include "Components/Spline.h"

#include <memory>

using namespace Pengine;

class Sponza : public Application
{
private:

	Spline* m_Spline = nullptr;
	std::shared_ptr<Camera> m_Camera;
	float m_PositionAlongSpline = 0.0f;
	float m_Time = 0.0f;
	class GameObject* m_Sphere = nullptr;
public:

	Sponza(const std::string& title = "Sponza")
		: Application(title)
	{}

	virtual void OnStart() override;

	virtual void OnPostStart() override {};

	virtual void OnUpdate() override;

	virtual void OnGuiRender() override;

	virtual void OnImGuiRender() override {};

	virtual void OnClose() override {};
};