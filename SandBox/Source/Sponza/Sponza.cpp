#include "Sponza.h"

#include "UI/Gui.h"
#include "Core/Environment.h"

void Sponza::OnStart()
{
	m_Camera = Environment::GetInstance().GetMainCamera();
	m_Spline = GetScene()->FindGameObjectByName("Spline")->m_ComponentManager.AddComponent<Spline>();
}

void Sponza::OnUpdate()
{
	if (m_Spline)
	{
		m_Time += Time::GetDeltaTime();
		m_PositionAlongSpline += m_Spline->m_Speed * Time::GetDeltaTime();
		glm::vec3 position = m_Spline->GetPoint(m_Spline->GetNormalizedOffset(m_PositionAlongSpline));
		m_Sphere = GetScene()->FindGameObjectByName("PointLight");
		m_Sphere->m_Transform.Translate(position);
	}
}

void Sponza::OnGuiRender()
{
	Gui& gui = Gui::GetInstance();
	gui.m_XAlign = Gui::AlignHorizontally::LEFT;
	gui.m_Theme.m_FontScale = 40.0f;
	gui.m_Theme.m_CharsAfterDot = 0;
	gui.Text(L"FPS: %f", { -gui.m_ViewportUISize.x * 0.5f, gui.m_ViewportUISize.y * 0.5f }, Time::GetAverageFps());
	gui.m_Theme.m_CharsAfterDot = 3;
	gui.Text(L"Frame time: %f", { -gui.m_ViewportUISize.x * 0.5f, gui.m_ViewportUISize.y * 0.5f - 40.0f - 5.0f }, Time::GetAverageDeltaTime() * 1000.0f);
	/*if (m_Spline)
	{
		gui.Text(L"T: %f", { -gui.m_ViewportUISize.x * 0.5f, gui.m_ViewportUISize.y * 0.5f - 120.0f - 15.0f }, m_Spline->GetNormalizedOffset(m_PositionAlongSpline));
		gui.Text(L"Position: %f", { -gui.m_ViewportUISize.x * 0.5f, gui.m_ViewportUISize.y * 0.5f - 160.0f - 20.0f }, m_PositionAlongSpline);
		gui.Text(L"Length: %f", { -gui.m_ViewportUISize.x * 0.5f, gui.m_ViewportUISize.y * 0.5f - 200.0f - 25.0f }, m_Spline->GetLength());
	
		gui.m_Theme.m_CharsAfterDot = 6;
		float distance = glm::distance(m_Sphere->m_Transform.GetPosition(), m_Sphere->m_Transform.GetPreviousPosition());
		gui.Text(L"Distance: %f", { -gui.m_ViewportUISize.x * 0.5f, gui.m_ViewportUISize.y * 0.5f - 240.0f - 30.0f }, distance);
	}*/
}
