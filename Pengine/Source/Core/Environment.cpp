#include "Environment.h"

#include "../EventSystem/EventSystem.h"

using namespace Pengine;

Environment::Environment()
{
#ifndef STANDALONE
	EventSystem::GetInstance().RegisterClient(EventType::ONSETSCROLL, this);
#endif
}

Environment& Environment::GetInstance()
{
	static Environment environment;
	return environment;
}

void Environment::OnSetScroll(const OnSetScrollEvent& event)
{
	m_MainCamera->SetZoom(event.GetOffset().y);
}

void Environment::SetDepthTest(bool depthTest)
{
	m_DepthTest = depthTest;

	if (depthTest)
	{
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
	}
	else
	{
		glDisable(GL_DEPTH_TEST);
	}
}
