#pragma once

#include "Core/Core.h"
#include "Core/Application.h"

using namespace Pengine;

class ExampleApplication : public Application
{
public:

	ExampleApplication(const std::string& title = "ExampleApplication")
		: Application(title)
	{}

	virtual void OnStart() override {};

	virtual void OnPostStart() override {};

	virtual void OnUpdate() override {};

	virtual void OnGuiRender() override {};

	virtual void OnImGuiRender() override {};

	virtual void OnClose() override {};
};